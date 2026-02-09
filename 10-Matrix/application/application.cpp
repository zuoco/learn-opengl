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
    return "Slint Application - Soft Renderer (Matrix)";
}

// 全局资源（与原始 main.cpp 中的逻辑一致）
static Image* texture = nullptr;
static bool prepared = false;
static Point p1, p2, p3;
static Point q1, q2, q3;

static float speed = 0.01;

static void changeUV()
{
    p1.uv.x += speed;
    p2.uv.x += speed;
    p3.uv.x += speed;
    q1.uv.x += speed;
    q2.uv.x += speed;
    q3.uv.x += speed;
}

static void prepare()
{
    texture = Image::createImage("assets/textures/goku.jpg");

    p1.x = 0;
    p1.y = 0;
    p1.color = RGBA(255, 0, 0, 255);
    p1.uv = math::vec2f(0.0f, 0.0f);

    p2.x = 400;
    p2.y = 300;
    p2.color = RGBA(0, 255, 0, 255);
    p2.uv = math::vec2f(1.0f, 1.0f);

    p3.x = 400;
    p3.y = 0;
    p3.color = RGBA(0, 0, 255, 255);
    p3.uv = math::vec2f(1.0f, 0.0f);

    q1.x = 0;
    q1.y = 0;
    q1.color = RGBA(255, 0, 0, 255);
    q1.uv = math::vec2f(0.0f, 0.0f);

    q2.x = 0;
    q2.y = 300;
    q2.color = RGBA(0, 255, 0, 255);
    q2.uv = math::vec2f(0.0f, 1.0f);

    q3.x = 400;
    q3.y = 300;
    q3.color = RGBA(0, 0, 255, 255);
    q3.uv = math::vec2f(1.0f, 1.0f);
}

void Application::render()
{
    if (!mAlive) return;

    // 首次渲染时准备数据
    if (!prepared) {
        prepare();
        prepared = true;
    }

    // UV 滚动动画（跑马灯效果）
    changeUV();

    sgl->clear();
    sgl->setTexture(texture);
    sgl->setTextureWrap(TEXTURE_WRAP_MIRROR);

    sgl->drawTriangle(p1, p2, p3);
    sgl->drawTriangle(q1, q2, q3);
}
