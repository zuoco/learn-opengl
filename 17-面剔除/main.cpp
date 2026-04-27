#include "slint.h"
#include "slint_image.h"
#include "slint_timer.h"
#include "app.h"
#include "gpu/gpu.h"
#include "gpu/dataStructures.h"
#include "gpu/shader/defaultShader.h"
#include "math/math.h"

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

uint32_t positionVbo = 0;
uint32_t colorVbo = 0;
uint32_t uvVbo = 0;
uint32_t ebo = 0;
uint32_t vao = 0;

DefaultShader* shader = nullptr;
math::mat4f modelMatrix;
math::mat4f viewMatrix;
math::mat4f perspectiveMatrix;

float angle = 0.0f;
float cameraZ = 2.0f;

void transform() {
	angle += 0.01f;

	modelMatrix = math::rotate(math::mat4f(1.0f), angle, math::vec3f{ 0.0f, 1.0f, 0.0f });

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
	sgl->bindVertexArray(vao);
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

	float positions[] = {
		-0.5f, 0.0f, 0.0f,
		0.5f, 0.0f, 0.0f,
		0.25f, 0.5f, 0.0f,
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
}

int main() {
	sgl->initSurface(WIDTH, HEIGHT, nullptr);
	prepare();

	auto ui = AppWindow::create();

	slint::Timer renderTimer(std::chrono::milliseconds(16), [&ui]() {
		render();

		auto fb = sgl->getFrameBuffer();
		if (!fb || !fb->mColorBuffer) {
			return;
		}

		// FrameBuffer stores pixels in RGBA format (matching slint::Rgba8Pixel)
		slint::SharedPixelBuffer<slint::Rgba8Pixel> pixelBuffer(
			fb->mWidth, fb->mHeight,
			reinterpret_cast<slint::Rgba8Pixel*>(fb->mColorBuffer)
		);

		slint::Image image(pixelBuffer);
		ui->set_framebuffer_image(image);
	});

	ui->show();
	slint::run_event_loop();

	delete shader;
	return 0;
}
