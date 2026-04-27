#include "gpu.h"
#include "raster.h"

GPU* GPU::mInstance = nullptr;
GPU* GPU::getInstance() {
	if (!mInstance) {
		mInstance = new GPU();
	}

	return mInstance;
}

GPU::GPU() {}

GPU::~GPU() {
	if (mFrameBuffer) {
		delete mFrameBuffer;
	}

	for (auto iter : mBufferMap) {
		delete iter.second;
	}
	mBufferMap.clear();

	for (auto iter : mVaoMap) {
		delete iter.second;
	}
	mVaoMap.clear();
}

void GPU::initSurface(const uint32_t& width, const uint32_t& height, void* buffer) {
	mFrameBuffer = new FrameBuffer(width, height, buffer);
}

void GPU::clear() {
	size_t pixelSize = mFrameBuffer->mWidth * mFrameBuffer->mHeight;
	std::fill_n(mFrameBuffer->mColorBuffer, pixelSize, RGBA(0, 0, 0, 0));
}

uint32_t GPU::genBuffer() {
	mBufferCounter++;
	mBufferMap.insert(std::make_pair(mBufferCounter, new BufferObject()));

	return mBufferCounter;
}

void GPU::deleteBuffer(const uint32_t& bufferID) {
	auto iter = mBufferMap.find(bufferID);
	if (iter != mBufferMap.end()) {
		delete iter->second;
	}
	else {
		return;
	}

	mBufferMap.erase(iter);
}

uint32_t GPU::genVertexArray() {
	mVaoCounter++;
	mVaoMap.insert(std::make_pair(mVaoCounter, new VertexArrayObject()));

	return mVaoCounter;
}

void GPU::deleteVertexArray(const uint32_t& vaoID) {
	auto iter = mVaoMap.find(vaoID);
	if (iter != mVaoMap.end()) {
		delete iter->second;
	}
	else {
		return;
	}

	mVaoMap.erase(iter);
}
