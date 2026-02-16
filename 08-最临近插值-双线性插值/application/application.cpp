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
    return "Slint Application - Soft Renderer (Bilinear)";
}

// 全局资源（与原始 main.cpp 中的逻辑一致）
static Image* texture = nullptr;
static bool prepared = false;
static Point p1, p2, p3;
static Point q1, q2, q3;

static void prepare()
{
    texture = Image::createImage("assets/textures/goku.jpg");

    p1.x = 0;
    p1.y = 0;
    p1.color = RGBA(255, 0, 0, 255);
    p1.uv = math::vec2f(0.0f, 0.0f);

    p2.x = 200;
    p2.y = 600;
    p2.color = RGBA(0, 255, 0, 255);
    p2.uv = math::vec2f(0.5f, 1.0f);

    p3.x = 400;
    p3.y = 0;
    p3.color = RGBA(0, 0, 255, 255);
    p3.uv = math::vec2f(1.0f, 0.0f);

    q1.x = 400;
    q1.y = 0;
    q1.color = RGBA(255, 0, 0, 255);
    q1.uv = math::vec2f(0.0f, 0.0f);

    q2.x = 600;
    q2.y = 600;
    q2.color = RGBA(0, 255, 0, 255);
    q2.uv = math::vec2f(0.5f, 1.0f);

    q3.x = 800;
    q3.y = 0;
    q3.color = RGBA(0, 0, 255, 255);
    q3.uv = math::vec2f(1.0f, 0.0f);
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

    // 左侧三角形：最近邻采样
    sgl->setBilinear(false);
    sgl->drawTriangle(p1, p2, p3);

    // 右侧三角形：双线性插值采样
    sgl->setBilinear(true);  // 启用双线性插值
    sgl->drawTriangle(q1, q2, q3);
}
