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

void Application::onKeyDown(char key) {
	if (mCamera) {
		mCamera->onKeyDown(key);
	}
}

void Application::onKeyUp(char key) {
	if (mCamera) {
		mCamera->onKeyUp(key);
	}
}

void Application::onRMouseDown(const int& x, const int& y) {
	if (mCamera) {
		mCamera->onRMouseDown(x, y);
	}
}

void Application::onRMouseUp(const int& x, const int& y) {
	if (mCamera) {
		mCamera->onRMouseUp(x, y);
	}
}

void Application::onMouseMove(const int& x, const int& y) {
	if (mCamera) {
		mCamera->onMouseMove(x, y);
	}
}

void Application::setCamera(Camera* camera) {
	mCamera = camera;
}
