#include "application.h"

Application* Application::mInstance = nullptr;

Application* Application::getInstance() {
    if (!mInstance) {
        mInstance = new Application();
    }
    return mInstance;
}

Application::Application() {}

Application::~Application() {}
