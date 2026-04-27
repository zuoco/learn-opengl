#include <iostream>
#include <chrono>
#include "slint.h"
#include "appwindow.h"
#include "application/application.h"
#include "gpu/gpu.h"
#include "gpu/dataStructures.h"
#include "gpu/shader/defaultShader.h"

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

//三角形顶点对应vbo
uint32_t positionVbo = 0;
uint32_t colorVbo = 0;
uint32_t uvVbo = 0;

//三角形的indices
uint32_t ebo = 0;

//三角形专属vao
uint32_t vao = 0;

//使用的Shader
DefaultShader* shader = nullptr;

//mvp变换矩阵
math::mat4f modelMatrix;
math::mat4f viewMatrix;
math::mat4f perspectiveMatrix;

float angle = 0.0f;
void transform() {
	angle += 0.01f;
	//模型变换
	modelMatrix = math::rotate(math::mat4f(1.0f), angle, math::vec3f{ 0.0f, 1.0f, 0.0f });
}

void render() {
	transform();
	shader->mModelMatrix = modelMatrix;
	shader->mViewMatrix = viewMatrix;
	shader->mProjectionMatrix = perspectiveMatrix;

	sgl->clear();
	sgl->useProgram(shader);
	sgl->bindVertexArray(vao);
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
	sgl->drawElement(DRAW_TRIANGLES, 0, 3);
}

void prepare() {
	shader = new DefaultShader();

	perspectiveMatrix = math::perspective(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

	auto cameraModelMatrix = math::translate(math::mat4f(1.0f), math::vec3f{ 0.0f, 0.0f, 3.0f });
	viewMatrix = math::inverse(cameraModelMatrix);

	float positions[] = {
		-0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
	};

	float colors[] = {
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
	};

	float uvs[] = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
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
	
	//创建每个vbo，绑定后填充数据设置ID，释放即可
	auto positionVbo = sgl->genBuffer();
	sgl->bindBuffer(ARRAY_BUFFER, positionVbo);
	sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 9, positions);
	sgl->vertexAttributePointer(0, 3, 3 * sizeof(float), 0);

	auto colorVbo = sgl->genBuffer();
	sgl->bindBuffer(ARRAY_BUFFER, colorVbo);
	sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 12, colors);
	sgl->vertexAttributePointer(1, 4, 4 * sizeof(float), 0);

	auto uvVbo = sgl->genBuffer();
	sgl->bindBuffer(ARRAY_BUFFER, uvVbo);
	sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 6, uvs);
	sgl->vertexAttributePointer(2, 2, 2 * sizeof(float), 0);

	sgl->bindBuffer(ARRAY_BUFFER, 0);
	sgl->bindVertexArray(0);
}

int main(int argc, char** argv) {
	//初始化GPU渲染表面
	sgl->initSurface(WIDTH, HEIGHT);

	prepare();

	auto ui = AppWindow::create();

	//使用Slint的Timer驱动渲染循环，约60FPS
	slint::Timer renderTimer(std::chrono::milliseconds(16), [&]() {
		render();

		//将FrameBuffer的像素数据转换为Slint Image
		//注意：项目中的RGBA布局现在是R,G,B,A，与slint::Rgba8Pixel一致
		slint::SharedPixelBuffer<slint::Rgba8Pixel> pixelBuffer(
			WIDTH, HEIGHT,
			reinterpret_cast<slint::Rgba8Pixel*>(sgl->getColorBuffer())
		);
		slint::Image image(pixelBuffer);
		ui->set_render_image(image);
	});

	ui->run();

	delete shader;

	return 0;
}
