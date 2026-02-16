#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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

    //stbimage读入的图片,原点在左上角，y轴是向下生长的
    //我方图形程序认为，图片应该是左下角为0，0；故需要翻转y轴
    stbi_set_flip_vertically_on_load(true);  // 传入 `false` (零值)：加载图像时不进行垂直翻转

    // 从文件路径加载图像： width和height存储图像宽度高度
    // picType： 图片的实际通道数
    // STBI_rgb_alpha： 请求的通道数
    unsigned char* bits = stbi_load(path.c_str(), &width, &height, &picType, STBI_rgb_alpha);
    
    // 由于图片是BGRA的格式，Image是RGBA的格式，所以得交换下R和B
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
