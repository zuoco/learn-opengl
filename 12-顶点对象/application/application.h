#pragma once
#include "../global/base.h"
#include <optional>
#include "slint.h"
#include "app.h"

#define app Application::getInstance()

class Application {
public:
	static Application* getInstance();
	Application();
	~Application();

	bool initApplication(const uint32_t& width = 800, const uint32_t& height = 600);
	void run();
	void shutdown();

	void updateRenderImage(const slint::Image& image);

	uint32_t getWidth() const { return mWidth; }
	uint32_t getHeight() const { return mHeight; }

private:
	static Application* mInstance;

	bool mAlive{ true };
	uint32_t mWidth = 800;
	uint32_t mHeight = 600;

	std::optional<slint::ComponentHandle<ui::AppWindow>> mAppWindow;
};
