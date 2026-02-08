#pragma once

#include <iostream>

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
