#include "frameBuffer.h"

FrameBuffer::FrameBuffer(uint32_t width, uint32_t height) {
	mWidth = width;
	mHeight = height;
	mColorBuffer = new RGBA[width * height];
}

FrameBuffer::~FrameBuffer() {
	if (mColorBuffer) {
		delete[] mColorBuffer;
	}
}
