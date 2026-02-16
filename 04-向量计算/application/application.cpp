#include "application.h"
#include "../gpu/gpu.h"
#include <cstdlib>
#include <ctime>

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
    return "Slint Application - Soft Renderer (Vectors)";
}

void Application::render()
{
    if (!mAlive) return;

    // 清除画布
    sgl->clear();

    // 绘制以屏幕中心为圆心的放射线
    int r = 150;
    Point c{ 400, 300, RGBA(255, 0, 0, 255) };

    for (float i = 0; i < 360; i += 10) {
        float radian = DEG2RAD(i);
        int x = r * sin(radian) + c.x;
        int y = r * cos(radian) + c.y;

        Point pt{ x, y, RGBA(static_cast<byte>(std::rand() % 255),
                             static_cast<byte>(std::rand() % 255),
                             static_cast<byte>(std::rand() % 255), 255) };

        sgl->drawLine(c, pt);
    }
}
