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

void Application::setCamera(Camera* camera) {
	mCamera = camera;
}

void Application::onKeyDown(const std::string& key) {
	if (mCamera) {
		mCamera->onKeyDown(key);
	}
}

void Application::onKeyUp(const std::string& key) {
	if (mCamera) {
		mCamera->onKeyUp(key);
	}
}

void Application::onMouseDown(float x, float y) {
	if (mCamera) {
		mCamera->onMouseDown(x, y);
	}
}

void Application::onMouseUp(float x, float y) {
	if (mCamera) {
		mCamera->onMouseUp(x, y);
	}
}

void Application::onMouseMove(float x, float y) {
	if (mCamera) {
		mCamera->onMouseMove(x, y);
	}
}
