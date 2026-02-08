#include <Windows.h>
#include<iostream>
#include "application/application.h"
#include "gpu/gpu.h"
#include "gpu/dataStructures.h"
#include "gpu/shader/lightShader.h"
#include "application/image.h"
#include "application/camera.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup" )

/*
* 课程内容介绍：
* 1 增加顶点法线
* 2 lightShader介绍
* 3 案例介绍
*/
uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

Camera* camera = nullptr;

//两个三角形，三个属性对应vbo
uint32_t interleavedVbo = 0;

//三角形的indices
uint32_t ebo = 0;

//两个三角形专属vao
uint32_t vao = 0;

//使用的Shader
LightShader* shader = nullptr;
DirectionalLight directionalLight;
math::vec3f envLight;

//使用的texture
uint32_t texture = 0;
Image* image = nullptr;

//mvp变换矩阵
math::mat4f modelMatrix;
void transform() {
	modelMatrix = math::translate(math::mat4f(), 0.0f, 0.0f, -4.0f);
}

void render() {
	transform();

	shader->mModelMatrix = modelMatrix;
	shader->mViewMatrix = camera->getViewMatrix();
	shader->mProjectionMatrix = camera->getProjectionMatrix();
	shader->mDiffuseTexture = texture;

	shader->mDirectionalLight = directionalLight;
	shader->mEnvLight = envLight;

	sgl->clear();
	sgl->useProgram(shader);

	sgl->bindVertexArray(vao);
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
	sgl->drawElement(DRAW_TRIANGLES, 0, 36);

}

void prepare() {
	camera = new Camera(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f, { 0.0f, 1.0f, 0.0f });
	app->setCamera(camera);
	camera->setSpeed(0.02f);

	shader = new LightShader();
	directionalLight.color = {1.0f, 1.0f, 1.0f};
	directionalLight.direction = {-1.0f, -0.3f, -0.7f};
	envLight = {0.1f, 0.1f, 0.1f};

	//制造纹理
	image = Image::createImage("assets/textures/goku.jpg");
	texture = sgl->genTexture();
	sgl->bindTexture(texture);
	sgl->texImage2D(image->mWidth, image->mHeight, image->mData);
	sgl->texParameter(TEXTURE_FILTER, TEXTURE_FILTER_LINEAR);
	sgl->texParameter(TEXTURE_WRAP_U, TEXTURE_WRAP_REPEAT);
	sgl->texParameter(TEXTURE_WRAP_V, TEXTURE_WRAP_REPEAT);
	sgl->bindTexture(0);

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	uint32_t indices[] = {
		0, 1, 2, 3, 4, 5,
		6, 7, 8, 9, 10, 11,
		12,13,14,15,16,17,
		18,19,20,21,22,23,
		24,25,26,27,28,29,
		30,31,32,33,34,35
	};

	//生成indices对应ebo
	ebo = sgl->genBuffer();
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
	sgl->bufferData(ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 36, indices);
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, 0);

	//总的交叉式vbo
	interleavedVbo = sgl->genBuffer();
	sgl->bindBuffer(ARRAY_BUFFER, interleavedVbo);
	sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 288, vertices);

	//生成vao并且绑定
	vao = sgl->genVertexArray();
	sgl->bindVertexArray(vao);
	sgl->bindBuffer(ARRAY_BUFFER, interleavedVbo);

	//position
	sgl->vertexAttributePointer(0, 3, 8 * sizeof(float), 0);

	//color
	sgl->vertexAttributePointer(1, 4, 8 * sizeof(float), 3 * sizeof(float));

	//uv
	sgl->vertexAttributePointer(2, 2, 8 * sizeof(float), 6 * sizeof(float));

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
		camera->update();

		render();
		app->show();
	}

	delete shader;
	Image::destroyImage(image);
	sgl->deleteTexture(texture);
	delete camera;

	return 0;
}
