#include "main.h"
#include "application/application.h"
#include "gpu/gpu.h"

/*
 * 课程内容介绍：
 * 讲授三维变换的实现（缩放/平移/旋转）
 * 讲授视图变换与投影变换
 * 实现一个三维旋转的三角形
 */

// 3D变换相关变量
Point p1;
Point p2;
Point p3;

math::vec4f pos1;
math::vec4f pos2;
math::vec4f pos3;

math::mat4f modelMatrix;
math::mat4f viewMatrix;
math::mat4f perspectiveMatrix;
math::mat4f screenMatrix;

float angle = 0.0f;
float cameraPos = 5.0f;
uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

Image* texture;

void transform() {
    angle += 0.01f;
    cameraPos += 0.01f;

    // 模型变换 - 绕Y轴旋转
    modelMatrix = math::rotate(math::mat4f(1.0f), angle, math::vec3f{ 0.0f, 1.0f, 0.0f });

    // 模拟摄像机往后退
    auto cameraModelMatrix = math::translate(math::mat4f(1.0f), math::vec3f{ 0.0f, 0.0f, cameraPos });
    viewMatrix = math::inverse(cameraModelMatrix);

    // MVP矩阵相乘
    auto sp1 = perspectiveMatrix * viewMatrix * modelMatrix * pos1;
    auto sp2 = perspectiveMatrix * viewMatrix * modelMatrix * pos2;
    auto sp3 = perspectiveMatrix * viewMatrix * modelMatrix * pos3;

    // 透视除法（此处故意设计z！=0）
    sp1 /= sp1.w;
    sp2 /= sp2.w;
    sp3 /= sp3.w;

    // 屏幕空间
    sp1 = screenMatrix * sp1;
    sp2 = screenMatrix * sp2;
    sp3 = screenMatrix * sp3;

    p1.x = sp1.x;
    p1.y = sp1.y;

    p2.x = sp2.x;
    p2.y = sp2.y;

    p3.x = sp3.x;
    p3.y = sp3.y;
}

void render() {
    transform();

    sgl->clear();
    sgl->drawTriangle(p1, p2, p3);
}

void prepare() {
    texture = Image::createImage("assets/textures/goku.jpg");

    p1.color = RGBA(255, 0, 0, 255);
    p1.uv = math::vec2f(0.0f, 0.0f);

    p2.color = RGBA(0, 255, 0, 255);
    p2.uv = math::vec2f(1.0f, 1.0f);

    p3.color = RGBA(0, 0, 255, 255);
    p3.uv = math::vec2f(1.0f, 0.0f);

    pos1 = { -1.5f, 0.0f, 0.0f, 1.0f };
    pos2 = { 1.5f, 0.0f, 0.0f, 1.0f };
    pos3 = { 0.0f, 2.0f, 0.0f, 1.0f };

    perspectiveMatrix = math::perspective(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    screenMatrix = math::screenMatrix<float>(WIDTH, HEIGHT);
}

int main(int argc, char *argv[])
{
    // 获取 Application 单例
    Application& app = Application::getInstance();

    // 初始化 GPU
    sgl->initSurface(app.getWidth(), app.getHeight());

    // 准备3D变换数据
    prepare();

    auto mainWindow = MainWindow::create();

    mainWindow->on_close_requested([&app]() {
        app.quit();
    });

    // 创建渲染定时器，约60fps
    slint::Timer renderTimer(std::chrono::milliseconds(16), [&mainWindow, &app]() {
        if (!app.isAlive()) return;

        // 渲染一帧
        render();

        // 从 GPU FrameBuffer 构建 Slint Image
        FrameBuffer* fb = sgl->getFrameBuffer();
        if (!fb) return;

        int width = fb->mWidth;
        int height = fb->mHeight;

        slint::SharedPixelBuffer<slint::Rgba8Pixel> pixelBuffer(width, height);
        auto *dest = pixelBuffer.begin();

        // 垂直翻转：GPU 从左下角开始绘制，Slint 从左上角开始
        for (int y = 0; y < height; ++y) {
            int srcY = height - 1 - y;
            for (int x = 0; x < width; ++x) {
                int srcPos = srcY * width + x;
                RGBA& pixel = fb->mColorBuffer[srcPos];
                int destPos = y * width + x;
                dest[destPos] = slint::Rgba8Pixel{ pixel.mR, pixel.mG, pixel.mB, pixel.mA };
            }
        }

        mainWindow->set_frame_buffer(slint::Image(pixelBuffer));
    });

    mainWindow->run();

    // 清理资源
    Image::destroyImage(texture);

    return 0;
}