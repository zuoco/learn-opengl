#include "application.h"

Application* Application::mInstance = nullptr;
Application* Application::getInstance() {
	if (mInstance == nullptr) {
		mInstance = new Application();
	}

	return mInstance;
}

Application::Application() {}
Application::~Application() {}

bool Application::initApplication(const uint32_t& width, const uint32_t& height) {
	mWidth = width;
	mHeight = height;

	mAppWindow = ui::AppWindow::create();
	(*mAppWindow)->show();

	return true;
}

void Application::updateRenderImage(const slint::Image& image) {
	if (mAppWindow.has_value()) {
		(*mAppWindow)->set_render_image(image);
	}
}

void Application::run() {
	(*mAppWindow)->run();
}

void Application::shutdown() {
	if (mAppWindow.has_value()) {
		(*mAppWindow)->hide();
	}
}
