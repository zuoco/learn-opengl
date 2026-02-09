#pragma once
#include "../global/base.h"
#include "frameBuffer.h"
#include "../application/image.h"
#include "../math/math.h"

#define sgl (&GPU::getInstance())

/*
 * class GPU:
 * 模拟GPU的绘图行为以及算法等
 */
class GPU {
public:
    static GPU& getInstance();

    ~GPU();

    // 接受外界传入的buffer对应的内存指针以及窗体的宽/高
    void initSurface(const uint32_t& width, const uint32_t& height, void* buffer = nullptr);

    // 清除画布内容
    void clear();

    // 传入像素位置，绘制成某种颜色
    void drawPoint(const uint32_t& x, const uint32_t& y, const RGBA& color);

    void drawLine(const Point& p1, const Point& p2);

    void drawTriangle(const Point& p1, const Point& p2, const Point& p3);

    void drawImage(const Image* image);

    void drawImageWidthAlpha(const Image* image, const uint32_t& alpha);

    // 设置状态
    void setBlending(bool enable);

    void setBilinear(bool enable);

    void setTexture(Image* image);

    FrameBuffer* getFrameBuffer() const { return mFrameBuffer; }

private:
    GPU();
    GPU(const GPU&) = delete;
    GPU& operator=(const GPU&) = delete;

    RGBA sampleNearest(const math::vec2f& uv);
    RGBA sampleBilinear(const math::vec2f& uv);

    bool mEnableBlending{ false };
    bool mEnableBilinear{ false };

    FrameBuffer* mFrameBuffer{ nullptr };

    // 纹理贴图
    Image* mImage{ nullptr };
};
