#include "gpu.h"
#include "raster.h"
#include <algorithm>

GPU& GPU::getInstance() {
    static GPU instance;
    return instance;
}

GPU::GPU() {}

GPU::~GPU() {
    if (mFrameBuffer) {
        delete mFrameBuffer;
    }
}

void GPU::initSurface(const uint32_t& width, const uint32_t& height, void* buffer) {
    if (mFrameBuffer) {
        delete mFrameBuffer;
    }
    mFrameBuffer = new FrameBuffer(width, height, buffer);
}

void GPU::clear() {
    size_t pixelSize = mFrameBuffer->mWidth * mFrameBuffer->mHeight;
    std::fill_n(mFrameBuffer->mColorBuffer, pixelSize, RGBA(0, 0, 0, 255));
}

void GPU::drawPoint(const uint32_t& x, const uint32_t& y, const RGBA& color) {
    if (x >= mFrameBuffer->mWidth || y >= mFrameBuffer->mHeight) {
        return;
    }

    // 从窗口左下角开始算起
    uint32_t pixelPos = y * mFrameBuffer->mWidth + x;
    mFrameBuffer->mColorBuffer[pixelPos] = color;
}

void GPU::drawLine(const Point& p1, const Point& p2) {
    std::vector<Point> pixels;
    Raster::rasterizeLine(pixels, p1, p2);

    for (auto& p : pixels) {
        drawPoint(p.x, p.y, p.color);
    }
}

void GPU::drawTriangle(const Point& p1, const Point& p2, const Point& p3) {
    std::vector<Point> pixels;
    // 三角形光栅化
    Raster::rasterizeTriangle(pixels, p1, p2, p3);

    for (auto& p : pixels) {
        drawPoint(p.x, p.y, p.color);
    }
}
