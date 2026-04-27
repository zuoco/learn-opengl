#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
#include "image.h"
#include <cstring>

Image::Image(const uint32_t& width, const uint32_t& height, RGBA* data) {
	mWidth = width;
	mHeight = height;
	if (data) {
		mData = new RGBA[mWidth * mHeight];
		memcpy(mData, data, sizeof(RGBA) * mWidth * mHeight);
	}
}

Image::~Image() {
	if (mData != nullptr) {
		delete[] mData;
	}
}

Image* Image::createImage(const std::string& path) {
	int picType = 0;
	int width{ 0 }, height{ 0 };

	//stbimage�����ͼƬ,ԭ�������Ͻǣ�y��������������
	//�ҷ�ͼ�γ�����Ϊ��ͼƬӦ�������½�Ϊ0��0������Ҫ��תy��
	stbi_set_flip_vertically_on_load(true);

	//����������BGRA�ĸ�ʽ��ͼƬ��RGBA�ĸ�ʽ�����Եý�����R&B
	unsigned char* bits = stbi_load(path.c_str(), &width, &height, &picType, STBI_rgb_alpha);
	for (int i = 0; i < width * height * 4; i += 4)
	{
		byte tmp = bits[i];
		bits[i] = bits[i + 2];
		bits[i + 2] = tmp;
	}

	Image* image = new Image(width, height, (RGBA*)bits);

	stbi_image_free(bits);

	return image;

}

void Image::destroyImage(Image* image) {
	if (image) {
		delete image;
	}
}
