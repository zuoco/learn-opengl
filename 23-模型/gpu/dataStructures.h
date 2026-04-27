#pragma once
#include <cstdint>
#include "../global/base.h"
#include "../math/math.h"

//VAO֮�У������������Զ�ȡ��ʽ��Description
struct BindingDescription {
	uint32_t	mVboId{ 0 };
	size_t		mItemSize{ 0 };
	size_t		mStride{ 0 };
	size_t		mOffset{ 0 };
};

struct VsOutput {
	float mOneOverW{ 0.0f };
	math::vec4f mPosition{ 0.0f, 0.0f, 0.0f, 1.0f };
	math::vec4f mColor;//�˴���ɫ��Ϊ0.0-1.0֮�����0-255����
	math::vec3f mNormal;
	math::vec2f mUV;
};

struct FsOutput {
	math::vec2i mPixelPos;
	float mDepth;
	RGBA mColor;//�˴�ʹ��0-255��������ɫ��ʾ
};
