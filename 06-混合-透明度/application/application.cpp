#include "application.h"
#include "../gpu/gpu.h"
#include "image.h"

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
    return "Slint Application - Soft Renderer (Image)";
}

// 全局图片资源（与原始 main.cpp 中的逻辑一致）
static Image* image01 = nullptr;
static Image* image02 = nullptr;
static bool imagesLoaded = false;

void Application::render()
{
    if (!mAlive) return;

    // 首次渲染时加载图片
    if (!imagesLoaded) {
        image01 = Image::createImage("assets/textures/me.png");
        image02 = Image::createImage("assets/textures/goku.jpg");
        imagesLoaded = true;
    }

    // 清除画布
    sgl->clear();

    // 开启 alpha 混合，绘制图片
    sgl->setBlending(true);
    sgl->drawImage(image02);
    sgl->drawImageWidthAlpha(image01, 100);
}
