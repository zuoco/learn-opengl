#include"application.h"

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
	return true;
}

void Application::peekMessage() {
	// In Slint-based application, event loop is handled by slint::run_event_loop()
	// This method is kept for compatibility with existing code structure
}
