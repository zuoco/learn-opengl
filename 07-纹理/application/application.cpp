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
    return "Slint Application - Soft Renderer (UV)";
}

// 全局资源

// 纹理
static Image* texture = nullptr;
static bool prepared = false;
static Point p1, p2, p3;

static void prepare()
{
    // 加载纹理
    texture = Image::createImage("assets/textures/goku.jpg");

    p1.x = 0;
    p1.y = 0;
    p1.color = RGBA(255, 0, 0, 255);
    p1.uv = math::vec2f(0.0f, 0.0f);

    p2.x = 400;
    p2.y = 900;
    p2.color = RGBA(0, 255, 0, 255);
    p2.uv = math::vec2f(0.5f, 1.0f);

    p3.x = 800;
    p3.y = 0;
    p3.color = RGBA(0, 0, 255, 255);
    p3.uv = math::vec2f(1.0f, 0.0f);
}

void Application::render()
{
    if (!mAlive) return;

    // 首次渲染时准备数据
    if (!prepared) {
        prepare();
        prepared = true;
    }

    sgl->clear();
    sgl->setTexture(texture);
    sgl->drawTriangle(p1, p2, p3);
}
