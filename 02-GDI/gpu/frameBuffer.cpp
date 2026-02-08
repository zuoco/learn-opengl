#include "frameBuffer.h"

FrameBuffer::FrameBuffer(uint32_t width, uint32_t height, void* buffer) {
    mWidth = width;
    mHeight = height;

    if (!buffer) {
        mColorBuffer = new RGBA[width * height];
        mExternBuffer = false;
    }
    else {
        mColorBuffer = static_cast<RGBA*>(buffer);
        mExternBuffer = true;
    }
}

FrameBuffer::~FrameBuffer() {
    if (!mExternBuffer && mColorBuffer) {
        delete[] mColorBuffer;
    }
}
