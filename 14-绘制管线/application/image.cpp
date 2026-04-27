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

	//stb_image读取图片,原点在左上角，y轴朝下
	//我方图像引擎认为图片应该以左下角为0，0，需要翻转y轴
	stbi_set_flip_vertically_on_load(true);

	//stb_image读出来的是RGBA的格式
	unsigned char* bits = stbi_load(path.c_str(), &width, &height, &picType, STBI_rgb_alpha);

	Image* image = new Image(width, height, (RGBA*)bits);

	stbi_image_free(bits);

	return image;

}

void Image::destroyImage(Image* image) {
	if (image) {
		delete image;
	}
}
