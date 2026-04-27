#include "application.h"
#include <cstring>

Application* Application::mInstance = nullptr;
Application* Application::getInstance() {
	if (mInstance == nullptr) {
		mInstance = new Application();
	}
	return mInstance;
}

Application::Application() {}

Application::~Application() {
	if (mCanvasBuffer) {
		delete[] static_cast<RGBA*>(mCanvasBuffer);
		mCanvasBuffer = nullptr;
	}
}

bool Application::init(const uint32_t& width, const uint32_t& height) {
	mWidth = width;
	mHeight = height;

	mCanvasBuffer = new RGBA[mWidth * mHeight];
	std::memset(mCanvasBuffer, 0, mWidth * mHeight * sizeof(RGBA));

	return true;
}
