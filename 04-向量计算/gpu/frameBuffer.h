#pragma once
#include "../global/base.h"

/*
 * class FrameBuffer:
 * 存储当前画布对应的内存指针，作为当前绘图画板
 */
class FrameBuffer {
public:
    FrameBuffer(uint32_t width, uint32_t height, void* buffer = nullptr);
    ~FrameBuffer();
    FrameBuffer(const FrameBuffer&) = delete;

    uint32_t    mWidth{ 0 };
    uint32_t    mHeight{ 0 };
    RGBA*       mColorBuffer{ nullptr };
    bool        mExternBuffer{ false };
};
