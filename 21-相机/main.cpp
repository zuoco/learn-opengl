#include <iostream>
#include <chrono>
#include "slint.h"
#include "slint_image.h"
#include "ui.h"
#include "application/camera.h"
#include "application/image.h"
#include "gpu/gpu.h"
#include "gpu/dataStructures.h"
#include "gpu/shader/textureShader.h"

/*
* 课程内容：
* 1 LookAt矩阵数学推导
* 2 Camera类的写法
* 3 Application的事件处理
*/
uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

Camera* camera = nullptr;

//顶点属性，分别对应vbo
uint32_t positionVbo = 0;

uint32_t colorVbo = 0;

uint32_t uvVbo = 0;

//三角形的indices
uint32_t ebo = 0;

//三角形专用vao
uint32_t vao = 0;

//使用的Shader
TextureShader* shader = nullptr;

//使用的texture
uint32_t texture = 0;
Image* image = nullptr;

//mvp变换矩阵
math::mat4f modelMatrix;

void render() {
	shader->mModelMatrix = modelMatrix;
	shader->mViewMatrix = camera->getViewMatrix();
	shader->mProjectionMatrix = camera->getProjectionMatrix();
	shader->mDiffuseTexture = texture;

	sgl->clear();
	sgl->useProgram(shader);

	sgl->bindVertexArray(vao);
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
	sgl->drawElement(DRAW_TRIANGLES, 0, 3);

}

void prepare() {
	camera = new Camera(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f, {0.0f, 1.0f, 0.0f});

	shader = new TextureShader();

	//加载纹理
	image = Image::createImage("assets/textures/goku.jpg");
	texture = sgl->genTexture();
	sgl->bindTexture(texture);
	sgl->texImage2D(image->mWidth, image->mHeight, image->mData);
	sgl->texParameter(TEXTURE_FILTER, TEXTURE_FILTER_LINEAR);
	sgl->texParameter(TEXTURE_WRAP_U, TEXTURE_WRAP_REPEAT);
	sgl->texParameter(TEXTURE_WRAP_V, TEXTURE_WRAP_REPEAT);
	sgl->bindTexture(0);

	sgl->enable(CULL_FACE);
	sgl->frontFace(FRONT_FACE_CCW);
	sgl->cullFace(BACK_FACE);

	float positions[] = {
		-0.5f, 0.0f, -1.0f,
		0.5f, 0.0f, -1.0f,
		0.0f, 0.5f, -1.0f,
	};

	float colors[] = {
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
	};

	float uvs[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.5f, 1.0f,
	};

	uint32_t indices[] = { 0, 1, 2 };

	//创建indices对应ebo
	ebo = sgl->genBuffer();
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
	sgl->bufferData(ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 3, indices);
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, 0);

	//创建vao并绑定
	vao = sgl->genVertexArray();
	sgl->bindVertexArray(vao);
	
	//position
	positionVbo = sgl->genBuffer();
	sgl->bindBuffer(ARRAY_BUFFER, positionVbo);
	sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 9, positions);
	sgl->vertexAttributePointer(0, 3, 3 * sizeof(float), 0);

	//color
	colorVbo = sgl->genBuffer();
	sgl->bindBuffer(ARRAY_BUFFER, colorVbo);
	sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 12, colors);
	sgl->vertexAttributePointer(1, 4, 4 * sizeof(float), 0);

	//uv
	uvVbo = sgl->genBuffer();
	sgl->bindBuffer(ARRAY_BUFFER, uvVbo);
	sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 6, uvs);
	sgl->vertexAttributePointer(2, 2, 2 * sizeof(float), 0);

	sgl->bindBuffer(ARRAY_BUFFER, 0);
	sgl->bindVertexArray(0);
}

int main(int argc, char* argv[])
{
	// 初始化GPU渲染表面（由GPU内部分配ColorBuffer）
	sgl->initSurface(WIDTH, HEIGHT, nullptr);

	prepare();

	auto ui = AppWindow::create();

	// 预分配像素缓冲区，避免每帧重新分配内存
	slint::SharedPixelBuffer<slint::Rgba8Pixel> pixelBuffer(WIDTH, HEIGHT);

	slint::Timer renderTimer(std::chrono::milliseconds(16), [&]() {
		camera->update();

		render();

		// 将GPU的BGRA framebuffer转换为Slint的RGBA格式
		FrameBuffer* fb = sgl->getFrameBuffer();
		RGBA* src = fb->getColorBuffer();
		slint::Rgba8Pixel* dst = pixelBuffer.begin();

		for (uint32_t i = 0; i < WIDTH * HEIGHT; ++i) {
			dst[i].r = src[i].mR;
			dst[i].g = src[i].mG;
			dst[i].b = src[i].mB;
			dst[i].a = src[i].mA;
		}

		slint::Image image(pixelBuffer);
		ui->set_framebuffer_image(image);
	});

	ui->run();

	delete shader;
	Image::destroyImage(image);
	sgl->deleteTexture(texture);
	delete camera;

	return 0;
}
