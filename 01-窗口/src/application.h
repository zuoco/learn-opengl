#pragma once

#include <iostream>

/*
  这个 Application 类是一个轻量级的应用框架，专注于管理图形渲染应用的运行状态，并为渲染操作提供统一的入口点。
  它是这个软渲染（GDI）项目的主控制器，连接了应用层逻辑和底层图形渲染功能。
*/


class Application
{
public:
    static Application& getInstance();
    
    void quit();
    std::string getApplicationInfo() const;
    bool isAlive() const { return mAlive; }

private:
    Application() = default;
    ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    bool mAlive{true};
};
