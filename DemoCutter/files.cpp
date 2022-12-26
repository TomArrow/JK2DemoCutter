
// Lifted from jomme and simplified. No zip loading and whatnot because it's not necessary. Only barebones functionality.
// Also no searchpaths. What you enter as path is what you get.

// Original file notice:
/*****************************************************************************
 * name:		files.c
 *
 * desc:		handle based filesystem for Quake III Arena
 *
 * $Archive: /MissionPack/code/qcommon/files.c $
 * $Author: Zaphod $
 * $Revision: 81 $
 * $Modtime: 5/30/01 2:31p $
 * $Date: 5/30/01 2:31p $
 *
 *****************************************************************************/


#include "demoCut.h"
#include "files_lzma.hpp"


#define MAX_ZPATH			256
#include <string>
#include <mutex>

#include <direct.h>


typedef union qfile_gus {
	FILE* o;
	//unzFile		z;
} qfile_gut;

typedef struct qfile_us {
	qfile_gut	file;
	qboolean	unique;
} qfile_ut;

typedef struct compressedFileInfo_t {
	fileCompressionScheme_t compression;

	// For LZMA
	qboolean readMode;
	LZMAIncrementalCompressor* lzmaCompressor;
	LZMADecompressor* lzmaDecompressor;
};

typedef struct {
	qfile_ut	handleFiles;
	qboolean	handleSync;
	int			baseOffset;
	int			fileSize;
	int			zipFilePos;
	qboolean	zipFile;
	qboolean	streamed;
	char		name[MAX_ZPATH];

	// For LZMA compressed demos but could be used for other stuff too
	compressedFileInfo_t compressedFileInfo;

} fileHandleData_t;

std::recursive_mutex fshMutex;
static fileHandleData_t	fsh[MAX_FILE_HANDLES];

static FILE* FS_FileForHandle(fileHandle_t f) {

	if (f < 0 || f > MAX_FILE_HANDLES) {
		Com_Error(ERR_DROP, "FS_FileForHandle: out of reange");
	}
	if (fsh[f].zipFile == qtrue) {
		Com_Error(ERR_DROP, "FS_FileForHandle: can't get FILE on zip file");
	}
	if (!fsh[f].handleFiles.file.o) {
		Com_Error(ERR_DROP, "FS_FileForHandle: NULL");
	}

	return fsh[f].handleFiles.file.o;
}

/*
=================
FS_Write

Properly handles partial writes
=================
*/
int FS_Write(const void* buffer, int len, fileHandle_t h, qboolean ignoreCompression) {
	int		block, remaining;
	int		written;
	byte* buf;
	int		tries;
	FILE* f;
	/*
	if (!fs_searchpaths) {
		Com_Error(ERR_FATAL, "Filesystem call made without initialization\n");
	}*/

	if (!h) {
		return 0;
	}

	if (!ignoreCompression && fsh[h].compressedFileInfo.compression == FILECOMPRESSION_LZMA) { // Idk, I guess this should have better error handling or sth but there isn't much we can do tbh
		if (!fsh[h].compressedFileInfo.readMode) {

			fsh[h].compressedFileInfo.lzmaCompressor->addData((byte*)buffer, len);
			return len;
		}
		else {
			throw std::logic_error("Can't use FS_Write on LZMA stream that was opened for read.");
		}
	}


	f = FS_FileForHandle(h);
	buf = (byte*)buffer;

	remaining = len;
	tries = 0;
	while (remaining) {
		block = remaining;
		written = fwrite(buf, 1, block, f);
		if (written == 0) {
			if (!tries) {
				tries = 1;
			}
			else {
				Com_Printf("FS_Write: 0 bytes written\n");
				return 0;
			}
		}

		if (written == -1) {
			Com_Printf("FS_Write: -1 bytes written\n");
			return 0;
		}

		remaining -= written;
		buf += written;
	}
	if (fsh[h].handleSync) {
		fflush(f);
	}
	return len;
}




static fileHandle_t	FS_HandleForFile(void) {
	int		i;
	{
		std::lock_guard lock(fshMutex); // So we don't accidentally use the same handle for multiple files with multithreading.
		for (i = 1; i < MAX_FILE_HANDLES; i++) {
			if (fsh[i].handleFiles.file.o == NULL) {
				return i;
			}
		}
	}
	Com_Error(ERR_DROP, "FS_HandleForFile: none free");
	return 0;
}	

static void FS_ReplaceSeparators(char* path) {
	char* s;

	for (s = path; *s; s++) {
		if (*s == '/' || *s == '\\') {
			*s = PATH_SEP;
		}
	}
}
std::string FS_BuildOSPath(const char* base, const char* game, const char* qpath) {
	char	temp[MAX_OSPATH];
	char ospath[MAX_OSPATH];

	//if (!game || !game[0]) {
	//	game = fs_gamedir;
	//}

	//Com_sprintf(temp, sizeof(temp), "/%s", qpath);
	Com_sprintf(temp, sizeof(temp), "%s", qpath);
	FS_ReplaceSeparators(temp);
	Com_sprintf(ospath, sizeof(ospath), "%s%s", base, temp);

	return ospath;
}

void Sys_Mkdir(const char* path) {
	_mkdir(path);
}

qboolean FS_CreatePath(char* OSPath, qboolean quiet) {
	char* ofs;

	// make absolutely sure that it can't back up the path
	// FIXME: is c: allowed???
	// TA 2022-03-30: Don't really care, this is just a demo cutter tool
	/*if (strstr(OSPath, "..") || strstr(OSPath, "::")) {
#ifdef ALLOWRELPATHSDEBUG
		if (!quiet) {
			Com_Printf("WARNING: creating relative path. Only allowed in Debug mode. \"%s\"\n", OSPath);
		}
#else
		Com_Printf("WARNING: refusing to create relative path \"%s\"\n", OSPath);
		return qtrue;
#endif
	}*/

	for (ofs = OSPath + 1; *ofs; ofs++) {
		if (*ofs == PATH_SEP) {
			// create the directory
			*ofs = 0;
			Sys_Mkdir(OSPath);
			*ofs = PATH_SEP;
		}
	}
	return qfalse;
}


constexpr int lzmaHeaderSize = LZMAIncrementalCompressor::getHeaderSize();
byte lzmaDummyHeader[lzmaHeaderSize]{};

fileHandle_t FS_FOpenFileWrite(const char* filename, qboolean quiet, fileCompressionScheme_t compression) { // quiet parameter if we want to suppress messages. for example when creating a logfile (endless recursion otherwise)
	std::string			ospath;
	fileHandle_t	f;

	//if (!fs_searchpaths) {
	//	Com_Error(ERR_FATAL, "Filesystem call made without initialization\n");
	//}

	std::lock_guard lock(fshMutex);
	f = FS_HandleForFile();
	fsh[f].zipFile = qfalse;

	//ospath = FS_BuildOSPath(fs_homepath->string, fs_gamedir, filename);
	ospath = FS_BuildOSPath("", "", filename);

	//if (fs_debug->integer && !quiet) {
	//	Com_Printf("FS_FOpenFileWrite: %s\n", ospath.c_str());
	//}

	if (FS_CreatePath((char*)ospath.c_str(), quiet)) {
		return 0;
	}

	// enabling the following line causes a recursive function call loop
	// when running with +set logfile 1 +set developer 1
	//Com_DPrintf( "writing to: %s\n", ospath );
	//fsh[f].handleFiles.file.o = fopen(ospath.c_str(), "wb");
	fopen_s(&fsh[f].handleFiles.file.o,ospath.c_str(), "wb");

	Q_strncpyz(fsh[f].name,sizeof(fsh[f].name), filename, sizeof(fsh[f].name));

	fsh[f].handleSync = qfalse;
	if (!fsh[f].handleFiles.file.o) {
		f = 0;
	}

	fsh[f].compressedFileInfo.readMode = qfalse;

	if (compression) {
		fsh[f].compressedFileInfo.compression = compression;
		FS_Write(&compression,4,f,qtrue); // qtrue for yes, ignore compression on that one.
	}
	if (compression == FILECOMPRESSION_LZMA) {
		// We will write the header later when we know the full size of the content, which will be in FS_FCloseFile
		FS_Write(lzmaDummyHeader, lzmaHeaderSize, f, qtrue);
		fsh[f].compressedFileInfo.lzmaCompressor = new LZMAIncrementalCompressor(
			[f](const void* buf, size_t size) -> size_t {
				return (size_t)FS_Write(buf, size, f,qtrue); // Last parameter qtrue (ignoreCompression) so the stuff is just written to the file regardless.
			} 
		);
	}

	return f;
}

qboolean FS_FileExists(const char* file)
{
	FILE* f;
	std::string testpath;

	testpath = FS_BuildOSPath("", "", file);

	//f = fopen(testpath.c_str(), "rb");
	fopen_s(&f,testpath.c_str(), "rb");
	if (f) {
		fclose(f);
		return qtrue;
	}
	return qfalse;
}

qboolean FS_FileErase(const char* file)
{
	std::string testpath;

	testpath = FS_BuildOSPath("", "", file);

	return (qboolean)(_unlink(testpath.c_str()) == 0);
}

void FS_FCloseFile(fileHandle_t f) {
	//if (!fs_searchpaths) {
	//	Com_Error(ERR_FATAL, "Filesystem call made without initialization\n");
	//}

	std::lock_guard lock(fshMutex); // just to be safe...
	/*if (fsh[f].streamed) {
		Sys_EndStreamedFile(f);
	}
	if (fsh[f].zipFile == qtrue) {
		unzCloseCurrentFile(fsh[f].handleFiles.file.z);
		if (fsh[f].handleFiles.unique) {
			unzClose(fsh[f].handleFiles.file.z);
		}
		Com_Memset(&fsh[f], 0, sizeof(fsh[f]));
		return;
	}*/

	if (fsh[f].compressedFileInfo.compression == FILECOMPRESSION_LZMA) {
		if (fsh[f].compressedFileInfo.readMode) {

			delete fsh[f].compressedFileInfo.lzmaDecompressor; // Just a quick and dirty cleanup.
			fsh[f].compressedFileInfo.lzmaDecompressor = NULL;
		}
		else {

			lzmaHeader_t header = fsh[f].compressedFileInfo.lzmaCompressor->Finish();
			int64_t pos = ftell(fsh[f].handleFiles.file.o);
			fseek(fsh[f].handleFiles.file.o, 4, SEEK_SET);	// Go to the reserved space for the LZMA header
			FS_Write(header.header, sizeof(header.header), f, qtrue); // Write LZMA header
			fseek(fsh[f].handleFiles.file.o, pos, SEEK_SET); // Go back to end. Not that it matters?
			delete fsh[f].compressedFileInfo.lzmaCompressor;
			fsh[f].compressedFileInfo.lzmaCompressor = NULL;
		}
	}

	// we didn't find it as a pak, so close it as a unique file
	if (fsh[f].handleFiles.file.o) {
		fclose(fsh[f].handleFiles.file.o);
	}
	Com_Memset(&fsh[f], 0, sizeof(fsh[f]));
}
//v1.03 #define	PROTOCOL_VERSION	15
#define	PROTOCOL_VERSION	16	//v1.04


int FS_filelength(fileHandle_t f) {
	int		pos;
	int		end;
	FILE* h;

	if (!fsh[f].compressedFileInfo.compression || fsh[f].compressedFileInfo.compression == FILECOMPRESSION_RAW) {

		h = FS_FileForHandle(f);
		pos = ftell(h);
		fseek(h, 0, SEEK_END);
		end = ftell(h);
		fseek(h, pos, SEEK_SET);

		return fsh[f].compressedFileInfo.compression == FILECOMPRESSION_RAW ? end-4 : end; // The compression type files have 4 bytes reserved at the start for the compression type itself. Let's not count that toward the total size.
	}
	else if (fsh[f].compressedFileInfo.compression == FILECOMPRESSION_LZMA) {
		
		if (fsh[f].compressedFileInfo.readMode) {

			return fsh[f].compressedFileInfo.lzmaDecompressor->getDataSize();
		}
		else {
			throw std::logic_error("Can't use FS_Filelength on LZMA files opened in write mode.");
		}
	}
	else {
		throw std::logic_error("can't use FS_filelength here");
	}

}


int FS_FOpenFileRead(const char* filename, fileHandle_t* file, qboolean uniqueFILE, qboolean compressedType, fileCompressionScheme_t* compressionUsed) {
	std::string		netpath;
	long			hash;
	int				l;
	char demoExt[16];

	hash = 0;

	//if (!fs_searchpaths) {
	//	Com_Error(ERR_FATAL, "Filesystem call made without initialization\n");
	//}

	if (file == NULL) {
		Com_Error(ERR_FATAL, "FS_FOpenFileRead: NULL 'file' parameter passed\n");
	}

	if (!filename) {
		Com_Error(ERR_FATAL, "FS_FOpenFileRead: NULL 'filename' parameter passed\n");
	}

	Com_sprintf(demoExt, sizeof(demoExt), ".dm_%d", PROTOCOL_VERSION);
	// qpaths are not supposed to have a leading slash
	if (filename[0] == '/' || filename[0] == '\\') {
		filename++;
	}

	// make absolutely sure that it can't back up the path.
	// The searchpaths do guarantee that something will always
	// be prepended, so we don't need to worry about "c:" or "//limbo" 
	// TA 2022-03-30 dont care this is a demo cutting tool
	//if (strstr(filename, "..") || strstr(filename, "::")) {
	//	*file = 0;
	//	return -1;
	//}

	//
	// search through the path, one element at a time
	//

	std::lock_guard lock(fshMutex);
	*file = FS_HandleForFile();
	fsh[*file].handleFiles.unique = uniqueFILE;


	netpath = FS_BuildOSPath("", "", filename);
	//fsh[*file].handleFiles.file.o = fopen(netpath.c_str(), "rb");
	fopen_s(&fsh[*file].handleFiles.file.o,netpath.c_str(), "rb");

	fsh[*file].compressedFileInfo.readMode = qtrue;

	if (!fsh[*file].handleFiles.file.o) {
		Com_DPrintf("Can't find %s\n", filename);
		*file = 0;
		return -1;
	}
	else {
		Q_strncpyz(fsh[*file].name,sizeof(fsh[*file].name), filename, sizeof(fsh[*file].name));
		fsh[*file].zipFile = qfalse;

		if (compressedType) {
			FS_Read(&fsh[*file].compressedFileInfo.compression, 4, *file);
			fsh[*file].compressedFileInfo.compression = LittleLong(fsh[*file].compressedFileInfo.compression);
			if (compressionUsed) {
				*compressionUsed = fsh[*file].compressedFileInfo.compression;
			}
		}
		if (fsh[*file].compressedFileInfo.compression == FILECOMPRESSION_LZMA) {
			int tmp = *file;
			fsh[*file].compressedFileInfo.lzmaDecompressor = new LZMADecompressor(
				[tmp](void* buf, size_t size) -> size_t {
					return (size_t)FS_Read(buf, size, tmp, qtrue); // Last parameter qtrue (ignoreCompression) so the stuff is just written to the file regardless.
				}
			);
		}
		return FS_filelength(*file);

	}
	
}

int FS_Read(void* buffer, int len, fileHandle_t f, qboolean ignoreCompression) {
	int		block, remaining;
	int		read;
	byte* buf;
	int		tries;

	/*if (!fs_searchpaths) {
		Com_Error(ERR_FATAL, "Filesystem call made without initialization\n");
	}*/

	if (!f) {
		return 0;
	}

	if (!ignoreCompression && fsh[f].compressedFileInfo.compression == FILECOMPRESSION_LZMA) { // Idk, I guess this should have better error handling or sth but there isn't much we can do tbh
		if (fsh[f].compressedFileInfo.readMode) {

			return fsh[f].compressedFileInfo.lzmaDecompressor->get((byte*)buffer, len);
		}
		else {
			throw std::logic_error("Can't use FS_Read on LZMA stream that was opened for write.");
		}
	}

	buf = (byte*)buffer;
	//fs_readCount += len;

	if (fsh[f].zipFile == qfalse) {
		remaining = len;
		tries = 0;
		while (remaining) {
			block = remaining;
			read = fread(buf, 1, block, fsh[f].handleFiles.file.o);
			if (read == 0) {
				// we might have been trying to read from a CD, which
				// sometimes returns a 0 read on windows
				if (!tries) {
					tries = 1;
				}
				else {
					return len - remaining;	//Com_Error (ERR_FATAL, "FS_Read: 0 bytes read");
				}
			}

			if (read == -1) {
				Com_Error(ERR_FATAL, "FS_Read: -1 bytes read");
			}

			remaining -= read;
			buf += read;
		}
		return len;
	}
	return 0;
	/*else {
		return unzReadCurrentFile(fsh[f].handleFiles.file.z, buffer, len);
	}*/
}