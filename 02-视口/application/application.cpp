#include "application.h"
#include "../gpu/gpu.h"
#include <cstdlib>

Application& Application::getInstance()
{
    static Application instance;
    return instance;
}

void Application::quit()
{
    mAlive = false;
    std::cout << "Application closing..." << std::endl;
}

std::string Application::getApplicationInfo() const
{
    return "Slint Application - Soft Renderer (GDI)";
}

void Application::render()
{
    if (!mAlive) return;

    // 清除画布
    sgl->clear();

    // 渲染随机噪点（保持原有功能）
    for (int i = 0; i < mWidth; ++i) {
        for (int j = 0; j < mHeight; ++j) {
            uint32_t v = std::rand() % 255;
            RGBA color(v, v, v, v);
            sgl->drawPoint(i, j, color);
        }
    }
}
