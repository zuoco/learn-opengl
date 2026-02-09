#include "main.h"
#include "application/application.h"
#include "gpu/gpu.h"

/*
* 1 GDI 绘制环境搭建 -> Slint 环境
* 2 搭建模拟GPU的代码架构
* 3 点的绘制实践
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

    // 创建一个16毫秒间隔的定时器：帧率控制
    slint::Timer renderTimer(
        std::chrono::milliseconds(16), 
        
        // 这是一个定时器回调函数，每16毫秒（约60FPS）执行一次，负责将GPU渲染的内容显示到Slint UI界面上。
        [&mainWindow, &app]() 
        {
                if (!app.isAlive()) return;

                // 渲染一帧: 调用 Application::render() 方法生成一帧图像内容
                app.render();

                // 从 GPU帧缓冲区 获取渲染完成的帧缓冲区数据
                FrameBuffer* fb = sgl->getFrameBuffer();
                if (!fb) return;

                // 获取帧缓冲区的宽度和高度
                int width = fb->mWidth;
                int height = fb->mHeight;

                // 创建与帧缓冲区相同尺寸的Slint像素缓冲区
                slint::SharedPixelBuffer<slint::Rgba8Pixel> pixelBuffer(width, height);
                // 获取目标缓冲区的起始指针
                auto *dest = pixelBuffer.begin();

                // 垂直翻转：GPU 从左下角开始绘制，Slint 从左上角开始
                for (int y = 0; y < height; ++y) {
                    // 一行一行的遍历
                    // 当 y=0（第一行）时，srcY=height-1   （最后一行）
                    // 当 y=height-1（最后一行）时，srcY=0 （第一行）
                    int srcY = height - 1 - y;

                    for (int x = 0; x < width; ++x) {
                        // 当前行中的每一个像素的位置
                        int srcPos = srcY * width + x;
                        // 从帧缓冲区中拿到像素值
                        RGBA& pixel = fb->mColorBuffer[srcPos];
                        
                        //  计算该像素在屏幕上的位置
                        int destPos = y * width + x;
                        dest[destPos] = slint::Rgba8Pixel{ pixel.mR, pixel.mG, pixel.mB, pixel.mA };
                    }
                }

                // 将转换后的图像数据设置到主窗口的帧缓冲区
                // 这样UI就能显示最新的渲染结果
                mainWindow->set_frame_buffer(slint::Image(pixelBuffer));
        }
    );

    mainWindow->run();

    return 0;
}
