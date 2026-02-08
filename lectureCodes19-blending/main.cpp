#include <Windows.h>
#include<iostream>
#include "application/application.h"
#include "gpu/gpu.h"
#include "gpu/dataStructures.h"
#include "gpu/shader/defaultShader.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup" )

/*
* 课程内容介绍：
* 1 颜色混合状态
* 2 颜色混合加入绘制流程
* 3 案例
*/
uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

//两个三角形，三个属性对应vbo
uint32_t positionVbo0 = 0;
uint32_t positionVbo1 = 0;
uint32_t positionVbo2 = 0;

uint32_t colorVbo0 = 0;
uint32_t colorVbo1 = 0;
uint32_t colorVbo2 = 0;

uint32_t uvVbo = 0;

//三角形的indices
uint32_t ebo = 0;

//两个三角形专属vao
uint32_t vao0 = 0;
uint32_t vao1 = 0;
uint32_t vao2 = 0;

//使用的Shader
DefaultShader* shader = nullptr;

//mvp变换矩阵
math::mat4f modelMatrix;
math::mat4f viewMatrix;
math::mat4f perspectiveMatrix;


float angle = 0.0f;
float cameraZ = 2.0f;
void transform() {
	//angle += 0.01f;
	//cameraZ -= 0.01f;

	//模型变换
	modelMatrix = math::rotate(math::mat4f(1.0f), angle, math::vec3f{ 0.0f, 1.0f, 0.0f });

	//视图变换
	auto cameraModelMatrix = math::translate(math::mat4f(1.0f), math::vec3f{ 0.0f, 0.0f, cameraZ });
	viewMatrix = math::inverse(cameraModelMatrix);
}

void render() {
	transform();
	shader->mModelMatrix = modelMatrix;
	shader->mViewMatrix = viewMatrix;
	shader->mProjectionMatrix = perspectiveMatrix;

	sgl->clear();
	sgl->useProgram(shader);

	//非透明物体必须先绘制，z为-0.8
	sgl->bindVertexArray(vao1);
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
	sgl->drawElement(DRAW_TRIANGLES, 0, 3);

	//透明物后绘制,z为-0.5
	sgl->bindVertexArray(vao2);
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
	sgl->drawElement(DRAW_TRIANGLES, 0, 3);

	//透明物后绘制,z为0
	sgl->bindVertexArray(vao0);
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
	sgl->drawElement(DRAW_TRIANGLES, 0, 3);
	
}

void prepare() {
	shader = new DefaultShader();

	perspectiveMatrix = math::perspective(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

	auto cameraModelMatrix = math::translate(math::mat4f(1.0f), math::vec3f{ 0.0f, 0.0f, cameraZ });
	viewMatrix = math::inverse(cameraModelMatrix);

	sgl->enable(CULL_FACE);
	sgl->frontFace(FRONT_FACE_CCW);
	sgl->cullFace(BACK_FACE);

	sgl->enable(BLENDING);


	//第一个三角形
	float positions0[] = {
		-0.5f, 0.0f, 0.0f,
		0.5f, 0.0f, 0.0f,
		0.25f, 0.5f, 0.0f,
	};

	float colors0[] = {
		1.0f, 0.0f, 0.0f, 0.3f,
		0.0f, 1.0f, 0.0f, 0.3f,
		0.0f, 0.0f, 1.0f, 0.3f,
	};

	//第二个三角形
	float positions1[] = {
		0.3f, 0.0f, -0.8f,
		0.8f, 0.0f, -0.8f,
		0.45f, 0.5f, -0.8f,
	};

	float colors1[] = {
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
	};

	//第三个三角形
	float positions2[] = {
		0.5f, 0.0f, -0.5f,
		1.0f, 0.0f, -0.5f,
		0.75f, 0.5f, -0.5f,
	};

	float colors2[] = {
		0.0f, 0.0f, 1.0f, 0.5f,
		0.0f, 0.0f, 1.0f, 0.5f,
		0.0f, 0.0f, 1.0f, 0.5f,
	};

	float uvs[] = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
	};

	uint32_t indices[] = { 0, 1, 2 };

	//生成indices对应ebo, 二者公用
	ebo = sgl->genBuffer();
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
	sgl->bufferData(ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 3, indices);
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, 0);

	//生成uv对应的vbo，二者公用
	 uvVbo = sgl->genBuffer();
	sgl->bindBuffer(ARRAY_BUFFER, uvVbo);
	sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 6, uvs);
	sgl->bindBuffer(ARRAY_BUFFER, 0);

	//生成vao并且绑定
	vao0 = sgl->genVertexArray();
	sgl->bindVertexArray(vao0);
	
	//position0
	positionVbo0 = sgl->genBuffer();
	sgl->bindBuffer(ARRAY_BUFFER, positionVbo0);
	sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 9, positions0);
	sgl->vertexAttributePointer(0, 3, 3 * sizeof(float), 0);

	//color0
	colorVbo0 = sgl->genBuffer();
	sgl->bindBuffer(ARRAY_BUFFER, colorVbo0);
	sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 12, colors0);
	sgl->vertexAttributePointer(1, 4, 4 * sizeof(float), 0);

	//uv
	sgl->bindBuffer(ARRAY_BUFFER, uvVbo);
	sgl->vertexAttributePointer(2, 2, 2 * sizeof(float), 0);

	sgl->bindBuffer(ARRAY_BUFFER, 0);
	sgl->bindVertexArray(0);


	//生成vao并且绑定
	vao1 = sgl->genVertexArray();
	sgl->bindVertexArray(vao1);

	//position1
	positionVbo1 = sgl->genBuffer();
	sgl->bindBuffer(ARRAY_BUFFER, positionVbo1);
	sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 9, positions1);
	sgl->vertexAttributePointer(0, 3, 3 * sizeof(float), 0);

	//color0
	colorVbo1 = sgl->genBuffer();
	sgl->bindBuffer(ARRAY_BUFFER, colorVbo1);
	sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 12, colors1);
	sgl->vertexAttributePointer(1, 4, 4 * sizeof(float), 0);

	//uv
	sgl->bindBuffer(ARRAY_BUFFER, uvVbo);
	sgl->vertexAttributePointer(2, 2, 2 * sizeof(float), 0);

	sgl->bindBuffer(ARRAY_BUFFER, 0);
	sgl->bindVertexArray(0);

	//生成vao并且绑定
	vao2 = sgl->genVertexArray();
	sgl->bindVertexArray(vao2);

	//position1
	positionVbo2 = sgl->genBuffer();
	sgl->bindBuffer(ARRAY_BUFFER, positionVbo2);
	sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 9, positions2);
	sgl->vertexAttributePointer(0, 3, 3 * sizeof(float), 0);

	//color0
	colorVbo2 = sgl->genBuffer();
	sgl->bindBuffer(ARRAY_BUFFER, colorVbo2);
	sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 12, colors2);
	sgl->vertexAttributePointer(1, 4, 4 * sizeof(float), 0);

	//uv
	sgl->bindBuffer(ARRAY_BUFFER, uvVbo);
	sgl->vertexAttributePointer(2, 2, 2 * sizeof(float), 0);

	sgl->bindBuffer(ARRAY_BUFFER, 0);
	sgl->bindVertexArray(0);
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

	delete shader;

	return 0;
}
