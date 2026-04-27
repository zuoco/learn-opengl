#pragma once
#include "../global/base.h"
#include "camera.h"

#define APP Application::getInstance()

class Application {
public:
	static Application* getInstance();
	Application();
	~Application();

	void setCamera(Camera* camera);
	Camera* getCamera() const { return mCamera; }

	// input forwarding
	void onKeyDown(const std::string& key);
	void onKeyUp(const std::string& key);
	void onMouseDown(float x, float y);
	void onMouseUp(float x, float y);
	void onMouseMove(float x, float y);

private:
	static Application* mInstance;
	Camera* mCamera{ nullptr };
};
