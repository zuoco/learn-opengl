#include <slint.h>
#include "app-window.h"
#include <iostream>
#include "gpu/gpu.h"
#include "gpu/dataStructures.h"
#include "gpu/shader/lightShader.h"
#include "application/image.h"
#include "application/camera.h"
#include "application/model.h"
#include "application/application.h"

uint32_t WIDTH = 1200;
uint32_t HEIGHT = 900;

Camera* camera = nullptr;
LightShader* shader = nullptr;
DirectionalLight directionalLight;
math::vec3f envLight;
Model* model = nullptr;

float angle = 0;
void transform() {
	angle -= 0.02f;
	auto rotateMatrix = math::rotate(math::mat4f(), angle, { 0.0f, 1.0f, 0.0f });
	auto translateMatrix = math::translate(math::mat4f(), 0.0f, 0.0f, -5.0f);
	auto m = translateMatrix * rotateMatrix;

	m = math::scale(m, 0.1f, 0.1f, 0.1f);
	model->setModelMatrix(m);
}

void render() {
	transform();

	shader->mViewMatrix = camera->getViewMatrix();
	shader->mProjectionMatrix = camera->getProjectionMatrix();

	shader->mDirectionalLight = directionalLight;
	shader->mEnvLight = envLight;

	sgl->clear();
	sgl->useProgram(shader);
	model->draw(shader);
}

void prepare() {
	camera = new Camera(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f, { 0.0f, 1.0f, 0.0f });
	APP->setCamera(camera);

	shader = new LightShader();
	directionalLight.color = { 1.0f, 1.0f, 1.0f };
	directionalLight.direction = { -1.0f, -0.5f, -0.7f };
	envLight = { 0.5f, 0.5f, 0.5f };

	sgl->enable(CULL_FACE);

	model = new Model();
	model->read("assets/model/dinosaur/source/Rampaging T-Rex.glb");
	//model->read("assets/model/Fist Fight B.fbx");
	//model->read("assets/model/bag/backpack.obj");
}

int main(int argc, char* argv[]) {
	// Initialize GPU surface (internal buffer allocation)
	sgl->initSurface(WIDTH, HEIGHT);

	prepare();

	auto ui = AppWindow::create();

	// Register input callbacks
	ui->on_key_pressed([](const slint::SharedString& key) {
		APP->onKeyDown(std::string(key));
	});
	ui->on_key_released([](const slint::SharedString& key) {
		APP->onKeyUp(std::string(key));
	});
	ui->on_pointer_pressed([](float x, float y) {
		APP->onMouseDown(x, y);
	});
	ui->on_pointer_released([](float x, float y) {
		APP->onMouseUp(x, y);
	});
	ui->on_pointer_moved([](float x, float y) {
		APP->onMouseMove(x, y);
	});

	// Pre-allocate pixel buffer for framebuffer transfer
	slint::SharedPixelBuffer<slint::Rgba8Pixel> pixelBuffer(WIDTH, HEIGHT);

	// Rendering timer (~60fps)
	slint::Timer renderTimer(std::chrono::milliseconds(16), [&]() {
		camera->update();
		render();

		// Copy framebuffer to Slint image
		auto fb = sgl->getFrameBuffer();
		if (fb && fb->mColorBuffer) {
			std::memcpy(pixelBuffer.begin(), fb->mColorBuffer,
				WIDTH * HEIGHT * sizeof(slint::Rgba8Pixel));
			slint::Image image(pixelBuffer);
			ui->set_render_image(image);
		}
	});

	ui->show();
	slint::run_event_loop();

	delete shader;
	delete camera;
	delete model;

	return 0;
}
