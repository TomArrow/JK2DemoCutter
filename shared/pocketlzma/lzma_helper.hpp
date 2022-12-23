/* LzmaUtil.c -- Test application for LZMA compression
2021-11-01 : Igor Pavlov : Public domain */


#define LZMA_C_DEFINE
#include "pocketlzma/lzma_c.hpp"

#ifdef _WIN32
#define USE_WINDOWS_FILE
// #include <windows.h>
#endif

#ifdef USE_WINDOWS_FILE
#include <windows.h>
#else
// note: USE_FOPEN mode is limited to 32-bit file size
// #define USE_FOPEN
// #include <stdio.h>
#endif

EXTERN_C_BEGIN

/* ---------- File ---------- */

typedef struct
{
#ifdef USE_WINDOWS_FILE
    HANDLE handle;
#elif defined(USE_FOPEN)
    FILE* file;
#else
    int fd;
#endif
} CSzFile;

void File_Construct(CSzFile* p);
#if !defined(UNDER_CE) || !defined(USE_WINDOWS_FILE)
WRes InFile_Open(CSzFile* p, const char* name);
WRes OutFile_Open(CSzFile* p, const char* name);
#endif
#ifdef USE_WINDOWS_FILE
WRes InFile_OpenW(CSzFile* p, const WCHAR* name);
WRes OutFile_OpenW(CSzFile* p, const WCHAR* name);
#endif
WRes File_Close(CSzFile* p);

/* reads max(*size, remain file's size) bytes */
WRes File_Read(CSzFile* p, void* data, size_t* size);

/* writes *size bytes */
WRes File_Write(CSzFile* p, const void* data, size_t* size);

WRes File_Seek(CSzFile* p, Int64* pos, ESzSeek origin);
WRes File_GetLength(CSzFile* p, UInt64* length);


/* ---------- FileInStream ---------- */

typedef struct
{
    ISeqInStream vt;
    CSzFile file;
    WRes wres;
} CFileSeqInStream;

void FileSeqInStream_CreateVTable(CFileSeqInStream* p);


typedef struct
{
    ISeekInStream vt;
    CSzFile file;
    WRes wres;
} CFileInStream;

void FileInStream_CreateVTable(CFileInStream* p);


typedef struct
{
    ISeqOutStream vt;
    CSzFile file;
    WRes wres;
} CFileOutStream;

void FileOutStream_CreateVTable(CFileOutStream* p);

EXTERN_C_END



#ifndef USE_WINDOWS_FILE

#include <errno.h>

#ifndef USE_FOPEN
#include <stdio.h>
#include <fcntl.h>
#ifdef _WIN32
#include <io.h>
typedef int ssize_t;
typedef int off_t;
#else
#include <unistd.h>
#endif
#endif

#else

/*
   ReadFile and WriteFile functions in Windows have BUG:
   If you Read or Write 64MB or more (probably min_failure_size = 64MB - 32KB + 1)
   from/to Network file, it returns ERROR_NO_SYSTEM_RESOURCES
   (Insufficient system resources exist to complete the requested service).
   Probably in some version of Windows there are problems with other sizes:
   for 32 MB (maybe also for 16 MB).
   And message can be "Network connection was lost"
*/

#endif

#define kChunkSizeMax (1 << 22)

void File_Construct(CSzFile* p)
{
#ifdef USE_WINDOWS_FILE
    p->handle = INVALID_HANDLE_VALUE;
#elif defined(USE_FOPEN)
    p->file = NULL;
#else
    p->fd = -1;
#endif
}

#if !defined(UNDER_CE) || !defined(USE_WINDOWS_FILE)

static WRes File_Open(CSzFile* p, const char* name, int writeMode)
{
#ifdef USE_WINDOWS_FILE

    p->handle = CreateFileA(name,
        writeMode ? GENERIC_WRITE : GENERIC_READ,
        FILE_SHARE_READ, NULL,
        writeMode ? CREATE_ALWAYS : OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);
    return (p->handle != INVALID_HANDLE_VALUE) ? 0 : GetLastError();

#elif defined(USE_FOPEN)

    p->file = fopen(name, writeMode ? "wb+" : "rb");
    return (p->file != 0) ? 0 :
#ifdef UNDER_CE
        2; /* ENOENT */
#else
        errno;
#endif

#else

    int flags = (writeMode ? (O_CREAT | O_EXCL | O_WRONLY) : O_RDONLY);
#ifdef O_BINARY
    flags |= O_BINARY;
#endif
    p->fd = open(name, flags, 0666);
    return (p->fd != -1) ? 0 : errno;

#endif
}

WRes InFile_Open(CSzFile* p, const char* name) { return File_Open(p, name, 0); }

WRes OutFile_Open(CSzFile* p, const char* name)
{
#if defined(USE_WINDOWS_FILE) || defined(USE_FOPEN)
    return File_Open(p, name, 1);
#else
    p->fd = creat(name, 0666);
    return (p->fd != -1) ? 0 : errno;
#endif
}

#endif


#ifdef USE_WINDOWS_FILE
static WRes File_OpenW(CSzFile* p, const WCHAR* name, int writeMode)
{
    p->handle = CreateFileW(name,
        writeMode ? GENERIC_WRITE : GENERIC_READ,
        FILE_SHARE_READ, NULL,
        writeMode ? CREATE_ALWAYS : OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);
    return (p->handle != INVALID_HANDLE_VALUE) ? 0 : GetLastError();
}
WRes InFile_OpenW(CSzFile* p, const WCHAR* name) { return File_OpenW(p, name, 0); }
WRes OutFile_OpenW(CSzFile* p, const WCHAR* name) { return File_OpenW(p, name, 1); }
#endif

WRes File_Close(CSzFile* p)
{
#ifdef USE_WINDOWS_FILE

    if (p->handle != INVALID_HANDLE_VALUE)
    {
        if (!CloseHandle(p->handle))
            return GetLastError();
        p->handle = INVALID_HANDLE_VALUE;
    }

#elif defined(USE_FOPEN)

    if (p->file != NULL)
    {
        int res = fclose(p->file);
        if (res != 0)
        {
            if (res == EOF)
                return errno;
            return res;
        }
        p->file = NULL;
    }

#else

    if (p->fd != -1)
    {
        if (close(p->fd) != 0)
            return errno;
        p->fd = -1;
    }

#endif

    return 0;
}


WRes File_Read(CSzFile* p, void* data, size_t* size)
{
    size_t originalSize = *size;
    *size = 0;
    if (originalSize == 0)
        return 0;

#ifdef USE_WINDOWS_FILE

    do
    {
        const DWORD curSize = (originalSize > kChunkSizeMax) ? kChunkSizeMax : (DWORD)originalSize;
        DWORD processed = 0;
        const BOOL res = ReadFile(p->handle, data, curSize, &processed, NULL);
        data = (void*)((Byte*)data + processed);
        originalSize -= processed;
        *size += processed;
        if (!res)
            return GetLastError();
        // debug : we can break here for partial reading mode
        if (processed == 0)
            break;
    } while (originalSize > 0);

#elif defined(USE_FOPEN)

    do
    {
        const size_t curSize = (originalSize > kChunkSizeMax) ? kChunkSizeMax : originalSize;
        const size_t processed = fread(data, 1, curSize, p->file);
        data = (void*)((Byte*)data + (size_t)processed);
        originalSize -= processed;
        *size += processed;
        if (processed != curSize)
            return ferror(p->file);
        // debug : we can break here for partial reading mode
        if (processed == 0)
            break;
    } while (originalSize > 0);

#else

    do
    {
        const size_t curSize = (originalSize > kChunkSizeMax) ? kChunkSizeMax : originalSize;
        const ssize_t processed = read(p->fd, data, curSize);
        if (processed == -1)
            return errno;
        if (processed == 0)
            break;
        data = (void*)((Byte*)data + (size_t)processed);
        originalSize -= (size_t)processed;
        *size += (size_t)processed;
        // debug : we can break here for partial reading mode
        // break;
    } while (originalSize > 0);

#endif

    return 0;
}


WRes File_Write(CSzFile* p, const void* data, size_t* size)
{
    size_t originalSize = *size;
    *size = 0;
    if (originalSize == 0)
        return 0;

#ifdef USE_WINDOWS_FILE

    do
    {
        const DWORD curSize = (originalSize > kChunkSizeMax) ? kChunkSizeMax : (DWORD)originalSize;
        DWORD processed = 0;
        const BOOL res = WriteFile(p->handle, data, curSize, &processed, NULL);
        data = (const void*)((const Byte*)data + processed);
        originalSize -= processed;
        *size += processed;
        if (!res)
            return GetLastError();
        if (processed == 0)
            break;
    } while (originalSize > 0);

#elif defined(USE_FOPEN)

    do
    {
        const size_t curSize = (originalSize > kChunkSizeMax) ? kChunkSizeMax : originalSize;
        const size_t processed = fwrite(data, 1, curSize, p->file);
        data = (void*)((Byte*)data + (size_t)processed);
        originalSize -= processed;
        *size += processed;
        if (processed != curSize)
            return ferror(p->file);
        if (processed == 0)
            break;
    } while (originalSize > 0);

#else

    do
    {
        const size_t curSize = (originalSize > kChunkSizeMax) ? kChunkSizeMax : originalSize;
        const ssize_t processed = write(p->fd, data, curSize);
        if (processed == -1)
            return errno;
        if (processed == 0)
            break;
        data = (void*)((Byte*)data + (size_t)processed);
        originalSize -= (size_t)processed;
        *size += (size_t)processed;
    } while (originalSize > 0);

#endif

    return 0;
}


WRes File_Seek(CSzFile* p, Int64* pos, ESzSeek origin)
{
#ifdef USE_WINDOWS_FILE

    DWORD moveMethod;
    UInt32 low = (UInt32)*pos;
    LONG high = (LONG)((UInt64)*pos >> 16 >> 16); /* for case when UInt64 is 32-bit only */
    switch (origin)
    {
    case SZ_SEEK_SET: moveMethod = FILE_BEGIN; break;
    case SZ_SEEK_CUR: moveMethod = FILE_CURRENT; break;
    case SZ_SEEK_END: moveMethod = FILE_END; break;
    default: return ERROR_INVALID_PARAMETER;
    }
    low = SetFilePointer(p->handle, (LONG)low, &high, moveMethod);
    if (low == (UInt32)0xFFFFFFFF)
    {
        WRes res = GetLastError();
        if (res != NO_ERROR)
            return res;
    }
    *pos = ((Int64)high << 32) | low;
    return 0;

#else

    int moveMethod; // = origin;

    switch (origin)
    {
    case SZ_SEEK_SET: moveMethod = SEEK_SET; break;
    case SZ_SEEK_CUR: moveMethod = SEEK_CUR; break;
    case SZ_SEEK_END: moveMethod = SEEK_END; break;
    default: return EINVAL;
    }

#if defined(USE_FOPEN)
    {
        int res = fseek(p->file, (long)*pos, moveMethod);
        if (res == -1)
            return errno;
        *pos = ftell(p->file);
        if (*pos == -1)
            return errno;
        return 0;
    }
#else
    {
        off_t res = lseek(p->fd, (off_t)*pos, moveMethod);
        if (res == -1)
            return errno;
        *pos = res;
        return 0;
    }

#endif // USE_FOPEN
#endif // USE_WINDOWS_FILE
}


WRes File_GetLength(CSzFile* p, UInt64* length)
{
#ifdef USE_WINDOWS_FILE

    DWORD sizeHigh;
    DWORD sizeLow = GetFileSize(p->handle, &sizeHigh);
    if (sizeLow == 0xFFFFFFFF)
    {
        DWORD res = GetLastError();
        if (res != NO_ERROR)
            return res;
    }
    *length = (((UInt64)sizeHigh) << 32) + sizeLow;
    return 0;

#elif defined(USE_FOPEN)

    long pos = ftell(p->file);
    int res = fseek(p->file, 0, SEEK_END);
    *length = ftell(p->file);
    fseek(p->file, pos, SEEK_SET);
    return res;

#else

    off_t pos;
    *length = 0;
    pos = lseek(p->fd, 0, SEEK_CUR);
    if (pos != -1)
    {
        const off_t len2 = lseek(p->fd, 0, SEEK_END);
        const off_t res2 = lseek(p->fd, pos, SEEK_SET);
        if (len2 != -1)
        {
            *length = (UInt64)len2;
            if (res2 != -1)
                return 0;
        }
    }
    return errno;

#endif
}


/* ---------- FileSeqInStream ---------- */

static SRes FileSeqInStream_Read(const ISeqInStream* pp, void* buf, size_t* size)
{
    CFileSeqInStream* p = CONTAINER_FROM_VTBL(pp, CFileSeqInStream, vt);
    WRes wres = File_Read(&p->file, buf, size);
    p->wres = wres;
    return (wres == 0) ? SZ_OK : SZ_ERROR_READ;
}

void FileSeqInStream_CreateVTable(CFileSeqInStream* p)
{
    p->vt.Read = FileSeqInStream_Read;
}


/* ---------- FileInStream ---------- */

static SRes FileInStream_Read(const ISeekInStream* pp, void* buf, size_t* size)
{
    CFileInStream* p = CONTAINER_FROM_VTBL(pp, CFileInStream, vt);
    WRes wres = File_Read(&p->file, buf, size);
    p->wres = wres;
    return (wres == 0) ? SZ_OK : SZ_ERROR_READ;
}

static SRes FileInStream_Seek(const ISeekInStream* pp, Int64* pos, ESzSeek origin)
{
    CFileInStream* p = CONTAINER_FROM_VTBL(pp, CFileInStream, vt);
    WRes wres = File_Seek(&p->file, pos, origin);
    p->wres = wres;
    return (wres == 0) ? SZ_OK : SZ_ERROR_READ;
}

void FileInStream_CreateVTable(CFileInStream* p)
{
    p->vt.Read = FileInStream_Read;
    p->vt.Seek = FileInStream_Seek;
}


/* ---------- FileOutStream ---------- */

static size_t FileOutStream_Write(const ISeqOutStream* pp, const void* data, size_t size)
{
    CFileOutStream* p = CONTAINER_FROM_VTBL(pp, CFileOutStream, vt);
    WRes wres = File_Write(&p->file, data, &size);
    p->wres = wres;
    return size;
}

void FileOutStream_CreateVTable(CFileOutStream* p)
{
    p->vt.Write = FileOutStream_Write;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>




static const char* const kCantReadMessage = "Cannot read input file";
static const char* const kCantWriteMessage = "Cannot write output file";
static const char* const kCantAllocateMessage = "Cannot allocate memory";
static const char* const kDataErrorMessage = "Data error";


static int PrintError(char* buffer, const char* message)
{
    strcat(buffer, "\nError: ");
    strcat(buffer, message);
    strcat(buffer, "\n");
    return 1;
}

static int PrintError_WRes(char* buffer, const char* message, WRes wres)
{
    strcat(buffer, "\nError: ");
    strcat(buffer, message);
    sprintf(buffer + strlen(buffer), "\nSystem error code: %d", (unsigned)wres);
#ifndef _WIN32
    {
        const char* s = strerror(wres);
        if (s)
            sprintf(buffer + strlen(buffer), " : %s", s);
    }
#endif
    strcat(buffer, "\n");
    return 1;
}

static int PrintErrorNumber(char* buffer, SRes val)
{
    sprintf(buffer + strlen(buffer), "\n7-Zip error code: %d\n", (unsigned)val);
    return 1;
}

static int PrintUserError(char* buffer)
{
    return PrintError(buffer, "Incorrect command");
}


#define IN_BUF_SIZE (1 << 16)
#define OUT_BUF_SIZE (1 << 16)


static SRes Decode2(CLzmaDec* state, ISeqOutStream* outStream, ISeqInStream* inStream,
    UInt64 unpackSize)
{
    int thereIsSize = (unpackSize != (UInt64)(Int64)-1);
    Byte inBuf[IN_BUF_SIZE];
    Byte outBuf[OUT_BUF_SIZE];
    size_t inPos = 0, inSize = 0, outPos = 0;
    LzmaDec_Init(state);
    for (;;)
    {
        if (inPos == inSize)
        {
            inSize = IN_BUF_SIZE;
            RINOK(inStream->Read(inStream, inBuf, &inSize));
            inPos = 0;
        }
        {
            SRes res;
            SizeT inProcessed = inSize - inPos;
            SizeT outProcessed = OUT_BUF_SIZE - outPos;
            ELzmaFinishMode finishMode = LZMA_FINISH_ANY;
            ELzmaStatus status;
            if (thereIsSize && outProcessed > unpackSize)
            {
                outProcessed = (SizeT)unpackSize;
                finishMode = LZMA_FINISH_END;
            }

            res = LzmaDec_DecodeToBuf(state, outBuf + outPos, &outProcessed,
                inBuf + inPos, &inProcessed, finishMode, &status);
            inPos += inProcessed;
            outPos += outProcessed;
            unpackSize -= outProcessed;

            if (outStream)
                if (outStream->Write(outStream, outBuf, outPos) != outPos)
                    return SZ_ERROR_WRITE;

            outPos = 0;

            if (res != SZ_OK || (thereIsSize && unpackSize == 0))
                return res;

            if (inProcessed == 0 && outProcessed == 0)
            {
                if (thereIsSize || status != LZMA_STATUS_FINISHED_WITH_MARK)
                    return SZ_ERROR_DATA;
                return res;
            }
        }
    }
}


static SRes Decode(ISeqOutStream* outStream, ISeqInStream* inStream)
{
    UInt64 unpackSize;
    int i;
    SRes res = 0;

    CLzmaDec state;

    /* header: 5 bytes of LZMA properties and 8 bytes of uncompressed size */
    unsigned char header[LZMA_PROPS_SIZE + 8];

    /* Read and parse header */

    RINOK(SeqInStream_Read(inStream, header, sizeof(header)));

    unpackSize = 0;
    for (i = 0; i < 8; i++)
        unpackSize += (UInt64)header[LZMA_PROPS_SIZE + i] << (i * 8);

    LzmaDec_Construct(&state);
    RINOK(LzmaDec_Allocate(&state, header, LZMA_PROPS_SIZE, &g_Alloc));
    res = Decode2(&state, outStream, inStream, unpackSize);
    LzmaDec_Free(&state, &g_Alloc);
    return res;
}

static SRes Encode(ISeqOutStream* outStream, ISeqInStream* inStream, UInt64 fileSize, char* rs)
{
    CLzmaEncHandle enc;
    SRes res;
    CLzmaEncProps props;

    UNUSED_VAR(rs);

    enc = LzmaEnc_Create(&g_Alloc);
    if (enc == 0)
        return SZ_ERROR_MEM;

    LzmaEncProps_Init(&props);
    res = LzmaEnc_SetProps(enc, &props);

    if (res == SZ_OK)
    {
        Byte header[LZMA_PROPS_SIZE + 8];
        size_t headerSize = LZMA_PROPS_SIZE;
        int i;

        res = LzmaEnc_WriteProperties(enc, header, &headerSize);
        for (i = 0; i < 8; i++)
            header[headerSize++] = (Byte)(fileSize >> (8 * i));
        if (outStream->Write(outStream, header, headerSize) != headerSize)
            res = SZ_ERROR_WRITE;
        else
        {
            if (res == SZ_OK)
                res = LzmaEnc_Encode(enc, outStream, inStream, NULL, &g_Alloc, &g_Alloc);
        }
    }
    LzmaEnc_Destroy(enc, &g_Alloc, &g_Alloc);
    return res;
}


static int main2(int numArgs, const char* args[], char* rs)
{
    CFileSeqInStream inStream;
    CFileOutStream outStream;
    char c;
    int res;
    int encodeMode;
    BoolInt useOutFile = False;

    //LzFindPrepare();

    FileSeqInStream_CreateVTable(&inStream);
    File_Construct(&inStream.file);
    inStream.wres = 0;

    FileOutStream_CreateVTable(&outStream);
    File_Construct(&outStream.file);
    outStream.wres = 0;

    if (numArgs == 1)
    {
        //PrintHelp(rs);
        return 0;
    }

    if (numArgs < 3 || numArgs > 4 || strlen(args[1]) != 1)
        return PrintUserError(rs);

    c = args[1][0];
    encodeMode = (c == 'e' || c == 'E');
    if (!encodeMode && c != 'd' && c != 'D')
        return PrintUserError(rs);

    {
        size_t t4 = sizeof(UInt32);
        size_t t8 = sizeof(UInt64);
        if (t4 != 4 || t8 != 8)
            return PrintError(rs, "Incorrect UInt32 or UInt64");
    }

    {
        WRes wres = InFile_Open(&inStream.file, args[2]);
        if (wres != 0)
            return PrintError_WRes(rs, "Cannot open input file", wres);
    }

    if (numArgs > 3)
    {
        WRes wres;
        useOutFile = True;
        wres = OutFile_Open(&outStream.file, args[3]);
        if (wres != 0)
            return PrintError_WRes(rs, "Cannot open output file", wres);
    }
    else if (encodeMode)
        PrintUserError(rs);

    if (encodeMode)
    {
        UInt64 fileSize;
        WRes wres = File_GetLength(&inStream.file, &fileSize);
        if (wres != 0)
            return PrintError_WRes(rs, "Cannot get file length", wres);
        res = Encode(&outStream.vt, &inStream.vt, fileSize, rs);
    }
    else
    {
        res = Decode(&outStream.vt, useOutFile ? &inStream.vt : NULL);
    }

    if (useOutFile)
        File_Close(&outStream.file);
    File_Close(&inStream.file);

    if (res != SZ_OK)
    {
        if (res == SZ_ERROR_MEM)
            return PrintError(rs, kCantAllocateMessage);
        else if (res == SZ_ERROR_DATA)
            return PrintError(rs, kDataErrorMessage);
        else if (res == SZ_ERROR_WRITE)
            return PrintError_WRes(rs, kCantWriteMessage, outStream.wres);
        else if (res == SZ_ERROR_READ)
            return PrintError_WRes(rs, kCantReadMessage, inStream.wres);
        return PrintErrorNumber(rs, res);
    }
    return 0;
}


int MY_CDECL main(int numArgs, const char* args[])
{
    char rs[1000] = { 0 };
    int res = main2(numArgs, args, rs);
    fputs(rs, stdout);
    return res;
}
