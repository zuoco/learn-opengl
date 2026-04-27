#include <iostream>
#include <chrono>
#include "slint.h"
#include "slint_image.h"
#include "ui.h"
#include "gpu/gpu.h"
#include "gpu/dataStructures.h"
#include "gpu/shader/defaultShader.h"

/*
* 课程内容：
* 1 颜色混合状态
* 2 颜色混合公式与参数
* 3 深度
*/
uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

//三个三角形，分别对应vbo
uint32_t positionVbo0 = 0;
uint32_t positionVbo1 = 0;
uint32_t positionVbo2 = 0;

uint32_t colorVbo0 = 0;
uint32_t colorVbo1 = 0;
uint32_t colorVbo2 = 0;

uint32_t uvVbo = 0;

//三个三角形的indices
uint32_t ebo = 0;

//三个三角形各自专用vao
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

	//不透明黄色三角形，z为-0.8，最后画
	sgl->bindVertexArray(vao1);
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
	sgl->drawElement(DRAW_TRIANGLES, 0, 3);

	//透明蓝色三角形,z为-0.5
	sgl->bindVertexArray(vao2);
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
	sgl->drawElement(DRAW_TRIANGLES, 0, 3);

	//透明红色三角形,z为0
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


	//第一个红色三角形
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

	//第二个黄色三角形
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

	//第三个蓝色三角形
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

	//创建indices对应ebo, 复用
	ebo = sgl->genBuffer();
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
	sgl->bufferData(ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 3, indices);
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, 0);

	//创建uv对应的vbo，复用
	 uvVbo = sgl->genBuffer();
	sgl->bindBuffer(ARRAY_BUFFER, uvVbo);
	sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 6, uvs);
	sgl->bindBuffer(ARRAY_BUFFER, 0);

	//创建vao并绑定
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


	//创建vao并绑定
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

	//创建vao并绑定
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

int main(int argc, char* argv[])
{
	// 初始化GPU渲染表面（由GPU内部分配ColorBuffer）
	sgl->initSurface(WIDTH, HEIGHT, nullptr);

	prepare();

	auto ui = AppWindow::create();

	// 预分配像素缓冲区，避免每帧重新分配内存
	slint::SharedPixelBuffer<slint::Rgba8Pixel> pixelBuffer(WIDTH, HEIGHT);

	slint::Timer renderTimer(std::chrono::milliseconds(16), [&]() {
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

	return 0;
}
