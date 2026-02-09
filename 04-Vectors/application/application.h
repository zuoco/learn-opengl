#pragma once

#include <iostream>

class Application
{
public:
    static Application& getInstance();

    void quit();
    std::string getApplicationInfo() const;
    bool isAlive() const { return mAlive; }

    int getWidth() const { return mWidth; }
    int getHeight() const { return mHeight; }

    // 渲染：绘制一帧内容到 GPU FrameBuffer
    void render();

private:
    Application() = default;
    ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    bool mAlive{true};
    int mWidth{800};
    int mHeight{600};
};

#define sglApp (&Application::getInstance())
