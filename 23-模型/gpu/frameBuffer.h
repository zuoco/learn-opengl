#pragma once
#include <cstdint>
#include "../global/base.h"

/*
* class FrameBuffer��
* �洢��ǰ������Ӧ��bmp���ڴ�ָ�룬��Ϊ��ǰ��ͼ����
*/
class FrameBuffer {
public:
	FrameBuffer(uint32_t width, uint32_t height, void* buffer = nullptr);
	~FrameBuffer();
	FrameBuffer(const FrameBuffer&) = delete;//��׼��������

	uint32_t	mWidth{ 0 };
	uint32_t	mHeight{ 0 };
	RGBA*		mColorBuffer{ nullptr };
	float*		mDepthBuffer{ nullptr };
	bool		mExternBuffer{ false };
};