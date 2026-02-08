#include <Windows.h>
#include<iostream>
#include "application/application.h"
#include "gpu/gpu.h"
#include "gpu/dataStructures.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup" )

/*
* 课程内容介绍：
* 代码修改：
* ——dataStructure重构
* ——math lerp函数系统化
* ——VBO
* ——VAO
* ——GPU重构
*/
uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;


void render() {
	sgl->clear();
}

uint32_t vbo = 0;
uint32_t vao = 0;

void prepare() {
	vbo = sgl->genBuffer();
	sgl->deleteBuffer(vbo);

	vao = sgl->genVertexArray();
	sgl->deleteVertexArray(vao);
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

	return 0;
}
