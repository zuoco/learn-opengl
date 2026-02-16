#include "application.h"
#include "../gpu/gpu.h"

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
    return "Slint Application - Soft Renderer (Triangles)";
}

void Application::render()
{
    if (!mAlive) return;

    // 清除画布
    sgl->clear();

    // 绘制彩色三角形（重心插值着色），居中于 800x600 画布
    Point p1{ 150, 100, RGBA(255, 0, 0, 255) };
    Point p2{ 650, 100, RGBA(0, 255, 0, 255) };
    Point p3{ 400, 500, RGBA(0, 0, 255, 255) };

    sgl->drawTriangle(p1, p2, p3);
}
