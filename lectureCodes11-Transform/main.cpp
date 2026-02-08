#include <Windows.h>
#include<iostream>
#include "application/application.h"
#include "gpu/gpu.h"
#include "gpu/dataStructures.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup" )

/*
* 课程内容介绍：
* 讲授三维变换的实现（缩放/平移/旋转）
* 讲授视图变换与投影变换
* 实现一个三维旋转的三角形
*/
uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

Image* texture;

Point p1;
Point p2;
Point p3;

math::vec4f pos1;
math::vec4f pos2;
math::vec4f pos3;

math::mat4f modelMatrix;
math::mat4f viewMatrix;
math::mat4f perspectiveMatrix;
math::mat4f screenMatrix;

float angle = 0.0f;
float cameraPos = 5.0f;
void transform() {
	angle += 0.01f;
	cameraPos += 0.01f;

	//模型变换
	modelMatrix = math::rotate(math::mat4f(1.0f), angle, math::vec3f{ 0.0f, 1.0f, 0.0f });

	//模拟摄像机往后退
	auto cameraModelMatrix = math::translate(math::mat4f(1.0f), math::vec3f{ 0.0f, 0.0f, cameraPos });
	viewMatrix = math::inverse(cameraModelMatrix);

	//mvp矩阵相乘
	auto sp1 = perspectiveMatrix * viewMatrix * modelMatrix * pos1;
	auto sp2 = perspectiveMatrix * viewMatrix * modelMatrix * pos2;
	auto sp3 = perspectiveMatrix * viewMatrix * modelMatrix * pos3;

	//透视除法（此处故意设计z！=0）
	sp1 /= sp1.w;
	sp2 /= sp2.w;
	sp3 /= sp3.w;

	//屏幕空间
	sp1 = screenMatrix * sp1;
	sp2 = screenMatrix * sp2;
	sp3 = screenMatrix * sp3;

	p1.x = sp1.x;
	p1.y = sp1.y;

	p2.x = sp2.x;
	p2.y = sp2.y;

	p3.x = sp3.x;
	p3.y = sp3.y;
}

void render() {
	transform();

	sgl->clear();
	sgl->drawTriangle(p1, p2, p3);
}

void prepare() {
	texture = Image::createImage("assets/textures/goku.jpg");

	p1.color = RGBA(255, 0, 0, 255);
	p1.uv = math::vec2f(0.0f, 0.0f);

	p2.color = RGBA(0, 255, 0, 255);
	p2.uv = math::vec2f(1.0f, 1.0f);

	p3.color = RGBA(0, 0, 255, 255);
	p3.uv = math::vec2f(1.0f, 0.0f);

	pos1 = {-1.5f, 0.0f, 0.0f, 1.0f};
	pos2 = {1.5f, 0.0f, 0.0f, 1.0f };
	pos3 = {0.0f, 2.0f, 0.0f, 1.0f };

	perspectiveMatrix = math::perspective(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	screenMatrix = math::screenMatrix<float>(WIDTH, HEIGHT);
}




int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,		//本应用程序实例句柄，唯一指代当前程序
	_In_opt_ HINSTANCE hPrevInstance,	//本程序前一个实例，一般是null
	_In_ LPWSTR    lpCmdLine,		//应用程序运行参数
	_In_ int       nCmdShow)		//窗口如何显示（最大化、最小化、隐藏），不需理会
{
	if (!app->initApplication(hInstance, WIDTH, HEIGHT)) {
		return -1;
	}

	//将bmp指向的内存配置到sgl当中 
	sgl->initSurface(app->getWidth(), app->getHeight(), app->getCanvas());

	prepare();

	bool alive = true;
	while (alive) {
		alive = app->peekMessage();
		render();
		app->show();
	}

	Image::destroyImage(texture);

	return 0;
}
