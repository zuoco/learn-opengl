#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
#include "image.h"

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
	// No longer swap R/B channels because we now use standard RGBA format on Linux

	Image* image = new Image(width, height, (RGBA*)bits);

	stbi_image_free(bits);

	return image;
}

Image* Image::createImageFromMemory(
	const std::string& path,
	unsigned char* dataIn,
	uint32_t widthIn,
	uint32_t heightIn
) {
	int picType = 0;
	int width{ 0 }, height{ 0 };

	//��¼���������ݵĴ�С
	uint32_t dataInSize = 0;

	//һ��fbxģ���п��ܴ������jpg������ѹ����ʽ��ͼƬ����£�height����Ϊ0��width�ʹ���������ͼƬ�Ĵ�С
	if (!heightIn) {
		dataInSize = widthIn;
	}
	else {
		dataInSize = widthIn * heightIn;
	}

	//���������õ���dataIn��������չ����λͼ���ݣ��п�����һ��jpg png�ȸ�ʽ��ͼƬ������
	unsigned char* bits = stbi_load_from_memory(dataIn, dataInSize, &width, &height, &picType, STBI_rgb_alpha);
	// No longer swap R/B channels because we now use standard RGBA format on Linux

	Image* image = new Image(width, height, (RGBA*)bits);

	return image;
}

void Image::destroyImage(Image* image) {
	if (image) {
		delete image;
	}
}
