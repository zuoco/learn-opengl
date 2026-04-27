#pragma once
#include "../global/base.h"

#define app Application::getInstance()

class Application {
public:
	static Application* getInstance();
	Application();
	~Application();

	bool init(const uint32_t& width = 800, const uint32_t& height = 600);

	uint32_t getWidth() const { return mWidth; }
	uint32_t getHeight() const { return mHeight; }
	void* getCanvas() const { return mCanvasBuffer; }

private:
	static Application* mInstance;

	uint32_t mWidth = 800;
	uint32_t mHeight = 600;

	void* mCanvasBuffer{ nullptr };
};
