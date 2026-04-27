#pragma once
#include "../global/base.h"

/*
* class FrameBuffer：
* 存储当前帧对应的bmp内存指针，作为当前绘制目标
*/
class FrameBuffer {
public:
	FrameBuffer(uint32_t width, uint32_t height);
	~FrameBuffer();
	FrameBuffer(const FrameBuffer&) = delete;//标准写法禁止

	uint32_t	mWidth{ 0 };
	uint32_t	mHeight{ 0 };
	RGBA*		mColorBuffer{ nullptr };
};
