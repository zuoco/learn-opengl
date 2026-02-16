#include "main.h"
#include "application/application.h"
#include "gpu/gpu.h"

/*
 * 课程内容介绍：
 * 1 讲授图片读取代码
 * 2 讲授图片显示代码
 * 3 讲授图片透明方案
 */

int main(int argc, char *argv[])
{
    // 获取 Application 单例
    Application& app = Application::getInstance();

    // 初始化 GPU
    sgl->initSurface(app.getWidth(), app.getHeight());

    auto mainWindow = MainWindow::create();

    mainWindow->on_close_requested([&app]() {
        app.quit();
    });

    // 创建渲染定时器，约60fps
    slint::Timer renderTimer(std::chrono::milliseconds(16), [&mainWindow, &app]() {
        if (!app.isAlive()) return;

        // 渲染一帧
        app.render();

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

    return 0;
}
