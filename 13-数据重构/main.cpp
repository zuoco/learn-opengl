#include <slint.h>
#include "app.h"
#include <iostream>
#include "application/application.h"
#include "gpu/gpu.h"
#include "gpu/dataStructures.h"

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

void render() {
	sgl->clear();
}

uint32_t positionVbo = 0;
uint32_t colorVbo = 0;
uint32_t uvVbo = 0;

uint32_t ebo = 0;

uint32_t vao = 0;

void prepare() {
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

	ebo = sgl->genBuffer();
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
	sgl->bufferData(ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 3, indices);
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, 0);

	vao = sgl->genVertexArray();
	sgl->bindVertexArray(vao);

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

	sgl->printVAO(vao);
}

int main(int argc, char* argv[]) {
	auto ui = AppWindow::create();

	if (!app->init(WIDTH, HEIGHT)) {
		return -1;
	}

	sgl->initSurface(app->getWidth(), app->getHeight(), app->getCanvas());

	prepare();

	// 初始化时先渲染一帧并显示
	render();
	slint::SharedPixelBuffer<slint::Rgba8Pixel> buffer(
		WIDTH, HEIGHT,
		reinterpret_cast<slint::Rgba8Pixel*>(app->getCanvas()));
	ui->set_framebuffer(slint::Image(buffer));

	// 使用定时器驱动每帧渲染
	slint::Timer timer;
	timer.start(slint::TimerMode::Repeated, std::chrono::milliseconds(16), [&]() {
		render();
		slint::SharedPixelBuffer<slint::Rgba8Pixel> frameBuffer(
			WIDTH, HEIGHT,
			reinterpret_cast<slint::Rgba8Pixel*>(app->getCanvas()));
		ui->set_framebuffer(slint::Image(frameBuffer));
	});

	ui->run();
	return 0;
}
