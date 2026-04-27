#include <iostream>
#include <chrono>
#include "slint.h"
#include "application/application.h"
#include "gpu/gpu.h"
#include "gpu/dataStructures.h"

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

void frameUpdate() {
	render();

	FrameBuffer* fb = sgl->getFrameBuffer();
	if (!fb || !fb->mColorBuffer) {
		return;
	}

	// Convert RGBA buffer to Slint Rgba8Pixel buffer
	// Current RGBA struct is {mR, mG, mB, mA}, Slint Rgba8Pixel is {r, g, b, a}
	// Layout matches, safe to reinterpret_cast
	slint::SharedPixelBuffer<slint::Rgba8Pixel> pixelBuffer(
		fb->mWidth, fb->mHeight,
		reinterpret_cast<slint::Rgba8Pixel*>(fb->mColorBuffer)
	);

	slint::Image image(pixelBuffer);
	app->updateRenderImage(image);
}

int main(int argc, char* argv[]) {
	if (!app->initApplication(WIDTH, HEIGHT)) {
		return -1;
	}

	// GPU init with internal colorBuffer
	sgl->initSurface(app->getWidth(), app->getHeight(), nullptr);

	prepare();

	// Use Timer to drive render loop, 16ms (~60fps) per frame
	slint::Timer renderTimer(std::chrono::milliseconds(16), frameUpdate);

	app->run();

	return 0;
}
