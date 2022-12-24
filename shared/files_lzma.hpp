#ifndef MSG_LZMA_H
#define MSG_LZMA_H

#include <chrono>
#include <thread>
#include <mutex>
#include <queue>
#include <demoCut.h>
#include <condition_variable>
#include <future>
#include <functional>

#define LZMA_C_DEFINE
#include "pocketlzma/lzma_c.hpp"


#define LZMA_MY_HEADER_SIZE LZMA_PROPS_SIZE + 8 // This isnt only the strict LZMA header, this also includes a 64 bit size of the compressed content. That's how we do it here. It could be done differently. Whatever.
typedef struct lzmaHeader_t {
	Byte header[LZMA_MY_HEADER_SIZE];
};

class LZMAIncrementalCompressor {

	ISeqInStream inStream{};
	ISeqOutStream outStream{};
	ICompressProgress progressReporter{};

	std::queue<byte> bytesToCompressFIFO;

	std::mutex compressionQueueMutex;
	std::condition_variable compressionQueueWait;

	bool finished = false;

	bool destroy = false;

	uint64_t progressInputSize = 0;
	uint64_t progressOutputSize = 0;

	std::function<size_t(const void*, size_t)> outputCallback;

	// dirty hack to make C callback work 
	static SRes progressCallback(const ICompressProgress* p, UInt64 inSize, UInt64 outSize) {
		constexpr size_t streamPointerOffset = offsetof(LZMAIncrementalCompressor, progressReporter);
		LZMAIncrementalCompressor* incrementalCompressorObject = (LZMAIncrementalCompressor*)((int64_t)p - (int64_t)streamPointerOffset);
		incrementalCompressorObject->updateProgress(inSize,outSize);
		return SZ_OK;
	}

	void updateProgress(uint64_t inSize, uint64_t outSize) {
		progressInputSize = inSize;
		progressOutputSize = outSize;
	}

	// dirty hack to make C callback work 
	static size_t dataOutputCallback(const ISeqOutStream* p, const void* buf, size_t size) {
		constexpr size_t streamPointerOffset = offsetof(LZMAIncrementalCompressor, outStream);
		LZMAIncrementalCompressor* incrementalCompressorObject = (LZMAIncrementalCompressor*)((int64_t)p - (int64_t)streamPointerOffset);
		return incrementalCompressorObject->dataOutput(buf,size);
	}

	size_t dataOutput(const void* buf, size_t size) {
		return outputCallback(buf, size);
	}

	// dirty hack to make C callback work
	static SRes dataInputCallback(const ISeqInStream* p, void* buf, size_t * size) {
		constexpr size_t streamPointerOffset = offsetof(LZMAIncrementalCompressor, inStream);
		LZMAIncrementalCompressor* incrementalCompressorObject = (LZMAIncrementalCompressor*)((int64_t)p - (int64_t)streamPointerOffset);
		return incrementalCompressorObject->dataInput(buf,size);
	}

	SRes dataInput(void* buf, size_t* size) {
		
		byte* byteBuffer = (byte*)buf;
		size_t requestedSize = *size;
		*size = 0;
		while (*size < requestedSize) {
			std::unique_lock<std::mutex> workerLock(compressionQueueMutex);

			while (bytesToCompressFIFO.size() > 0 && *size < requestedSize) {
				*byteBuffer = bytesToCompressFIFO.front();
				bytesToCompressFIFO.pop();
				byteBuffer++;
				(*size)++;
			}
			
			if (finished) {
				return SZ_OK;
			}

			compressionQueueWait.wait(workerLock);
		}
		return SZ_OK;
	}

	std::thread* workerThread;


	lzmaHeader_t header{};
	size_t headerSize = LZMA_PROPS_SIZE;
	size_t headerOffset = headerSize;

	size_t totalCurrentSize = 0;


	void doCompress() {
		inStream.Read = dataInputCallback;
		outStream.Write = dataOutputCallback;
		progressReporter.Progress = progressCallback;
		CLzmaEncHandle enc;
		SRes res;
		CLzmaEncProps props;

		enc = LzmaEnc_Create(&g_Alloc);
		//if (enc == 0)
		//	return SZ_ERROR_MEM;

		LzmaEncProps_Init(&props);

		props.level = 9;
		props.dictSize = 1 << 27;

		res = LzmaEnc_SetProps(enc, &props);

		if (res == SZ_OK)
		{
			int i;

			res = LzmaEnc_WriteProperties(enc, header.header, &headerOffset);

			if (res == SZ_OK)
				res = LzmaEnc_Encode(enc, &outStream, &inStream, &progressReporter, &g_Alloc, &g_Alloc);

		}

		if (res == SZ_OK) {
			// Finish the header
			int tmpHeaderOffset = headerOffset;
			for (int i = 0; i < 8; i++) {
				header.header[tmpHeaderOffset++] = (Byte)(totalCurrentSize >> (8 * i));
			}
		}

		LzmaEnc_Destroy(enc, &g_Alloc, &g_Alloc);
	}

public:
	constexpr static int getHeaderSize() {
		return LZMA_MY_HEADER_SIZE;
	}


	LZMAIncrementalCompressor(std::function<size_t(const void*, size_t)> outputCallbackA) {
		std::lock_guard<std::mutex> lock(compressionQueueMutex);
		outputCallback = outputCallbackA;
		workerThread = new std::thread  ( [this] { doCompress(); } );
	}

	void addData(const byte* data, size_t length) {
		{
			std::lock_guard<std::mutex> lock(compressionQueueMutex);
			if (finished) {
				throw std::logic_error("Cannot add data after compression is finished");
			}
			for (int i = 0; i < length; i++) {
				bytesToCompressFIFO.push(data[i]);
			}
		}
		totalCurrentSize += length;
		compressionQueueWait.notify_all();
	}
	lzmaHeader_t Finish() {
		{
			std::lock_guard<std::mutex> lock(compressionQueueMutex);
			finished = true;
		}
		compressionQueueWait.notify_all();
		workerThread->join();
		delete workerThread;
		return header;
	}
};



static SRes FileSeqInStream_Read(const ISeqInStream* pp, void* buf, size_t* size)
{
	return pp->Read(pp, buf, size);
}


class LZMADecompressor {

	ISeqInStream inStream{};
	ISeqOutStream outStream{};

	std::function<size_t(void*, size_t)> inputCallback;
	lzmaHeader_t header{};

	// dirty hack to make C callback work
	static SRes dataInputCallback(const ISeqInStream* p, void* buf, size_t* size) {
		constexpr size_t streamPointerOffset = offsetof(LZMADecompressor, inStream);
		LZMADecompressor* decompressorObject = (LZMADecompressor*)((int64_t)p - (int64_t)streamPointerOffset);
		return decompressorObject->dataInput(buf, size);
	}

	SRes dataInput(void* buf, size_t* size) {

		size_t howMuchWeGet = inputCallback((byte*)buf,*size);
		*size = howMuchWeGet;
		return SZ_OK;
	}

	std::queue<byte> outputQueue; // Let's multithread the decoding, nice!

	UInt64 unpackSize = 0;

	std::mutex decompressionOutputQueueMutex;
	std::condition_variable decompressionQueueWait;

	std::mutex outputProvideMutex;
	std::condition_variable outputProvideWait;

	bool finished = false;

#define IN_BUF_SIZE (1 << 16)
#define OUT_BUF_SIZE (1 << 16)
	void doDecompress() {
		int i;
		SRes res = 0;

		CLzmaDec state;



		LzmaDec_Construct(&state);
		res = LzmaDec_Allocate(&state, header.header, LZMA_PROPS_SIZE, &g_Alloc);
		if (res != SZ_OK) {
			throw std::exception("Couldn't do LZMA decoder allocation.");
		}

		//res = Decode2(&state, outStream, inStream, unpackSize);
		int thereIsSize = (unpackSize != (UInt64)(Int64)-1); // ?!?!? idk what the intent here is tbh
		Byte inBuf[IN_BUF_SIZE];
		Byte outBuf[OUT_BUF_SIZE];
		size_t inPos = 0, inSize = 0, outPos = 0;
		LzmaDec_Init(&state);

		for (;;)
		{
			std::unique_lock<std::mutex> decompLock(decompressionOutputQueueMutex);

			if (outputQueue.size() <= outputBufferSize-OUT_BUF_SIZE) {


				if (inPos == inSize)
				{
					inSize = IN_BUF_SIZE;
					res = inStream.Read(&inStream, inBuf, &inSize);
					if (res != SZ_OK) {
						return;
					}
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

					res = LzmaDec_DecodeToBuf(&state, outBuf + outPos, &outProcessed,
						inBuf + inPos, &inProcessed, finishMode, &status);
					inPos += inProcessed;
					outPos += outProcessed;
					unpackSize -= outProcessed;

					{
						std::lock_guard<std::mutex> outputQueuePushLock(outputProvideMutex);
						for (i = 0; i < outPos; i++) {
							outputQueue.push(outBuf[i]);
						}
					}
					outputProvideWait.notify_all();
					//if (outStream.Write(&outStream, outBuf, outPos) != outPos)
					//	return;// SZ_ERROR_WRITE;

					outPos = 0;

					if (res != SZ_OK || (thereIsSize && unpackSize == 0))
						return;// res;

					if (inProcessed == 0 && outProcessed == 0)
					{
						finished = true;

						if (thereIsSize || status != LZMA_STATUS_FINISHED_WITH_MARK)
							return;// SZ_ERROR_DATA;
						return;// res;
					}
				}
			}

			decompressionQueueWait.wait(decompLock); // Don't blindly keep reading data into the output buffer, just wait until more data is requested
		}


		LzmaDec_Free(&state, &g_Alloc);
	}

	std::thread* workerThread;
	size_t outputBufferSize = 0;

public:
	LZMADecompressor(std::function<size_t(void*, size_t)> inputCallbackA,size_t outputBufferSizeA = 2097152) { // 20 MB buffer by default
		inputCallback = inputCallbackA;
		inStream.Read = dataInputCallback;
		outputBufferSize = outputBufferSizeA;

		/* Read and parse header */
		size_t headerSize = sizeof(header.header);
		SRes res = inStream.Read(&inStream, header.header, &headerSize);
		if (res != SZ_OK) {
			throw std::exception("Couldn't read LZMA header.");
		}

		unpackSize = 0;
		for (int i = 0; i < 8; i++) {
			unpackSize += (UInt64)header.header[LZMA_PROPS_SIZE + i] << (i * 8);
		}

		workerThread = new std::thread([this] {doDecompress(); });
	}

	uint64_t getDataSize() {
		return unpackSize;
	}

	// Double locking in this one... yikes! :D
	// The reading thread gets locked by this thread so it doesn't read too much
	// This thread gets locked by the reading thread so it doesn't try to uselessly read from an empty output queue again and again.
	size_t get(byte* buf, size_t count) {

		size_t countGiven = 0;
		while (countGiven < count) {

			{
				std::lock_guard<std::mutex> getLock(decompressionOutputQueueMutex);
				while (outputQueue.size() > 0 && countGiven < count) {
					buf[countGiven] = outputQueue.front();
					outputQueue.pop();
					countGiven++;
				}
				if (finished || countGiven == count) break; // Input data is already fully decoded.
			}
			decompressionQueueWait.notify_all();
			// There's still more data coming. Wait for it.
			std::unique_lock<std::mutex> waitForMoreLock(outputProvideMutex);
			if (outputQueue.size() == 0) {
				outputProvideWait.wait(waitForMoreLock); // Wait so we don't spin this loop endlessly like idiots.
			}
		}
		return countGiven;
	}
	void Finish() {
		workerThread->join();
		delete workerThread;
	}
};

#endif