#pragma once
#include "../global/base.h"

#define app Application::getInstance()

class Application {
public:
	static Application* getInstance();
	Application();
	~Application();

	uint32_t getWidth() const { return mWidth; }
	uint32_t getHeight() const { return mHeight; }

private:
	static Application* mInstance;
	uint32_t mWidth = 800;
	uint32_t mHeight = 600;
};
