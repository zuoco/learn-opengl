#pragma once
#include "../global/base.h"
#include "camera.h"

#define app Application::getInstance()

class Application {
public:
	static Application* getInstance();
	Application();
	~Application();

	bool initApplication(const uint32_t& width = 800, const uint32_t& height = 600);

	// 处理输入事件，并转发给 Camera
	void onKeyDown(char key);
	void onKeyUp(char key);
	void onRMouseDown(const int& x, const int& y);
	void onRMouseUp(const int& x, const int& y);
	void onMouseMove(const int& x, const int& y);

	uint32_t getWidth() const { return mWidth; }
	uint32_t getHeight() const { return mHeight; }

	void setCamera(Camera* camera);

private:
	static Application* mInstance;

	Camera* mCamera{ nullptr };

	bool		mAlive{ true };
	uint32_t	mWidth = 800;
	uint32_t	mHeight = 600;
};
