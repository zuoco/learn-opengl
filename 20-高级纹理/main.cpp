#include<iostream>
#include <chrono>
#include "slint.h"
#include "app.h"
#include "application/application.h"
#include "gpu/gpu.h"
#include "gpu/dataStructures.h"
#include "gpu/shader/textureShader.h"
#include "application/image.h"

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

// position vbo
uint32_t positionVbo = 0;

uint32_t colorVbo = 0;

uint32_t uvVbo = 0;

// ebo
uint32_t ebo = 0;

// vao
uint32_t vao = 0;

// shader
TextureShader* shader = nullptr;

// texture
uint32_t texture = 0;
Image* image = nullptr;

// mvp matrices
math::mat4f modelMatrix;
math::mat4f viewMatrix;
math::mat4f perspectiveMatrix;


float angle = 0.0f;
float cameraZ = 1.0f;
void transform() {
	angle += 0.002f;

	// model transform
	modelMatrix = math::rotate(math::mat4f(1.0f), angle, math::vec3f{ 0.0f, 1.0f, 0.0f });

	// view transform
	auto cameraModelMatrix = math::translate(math::mat4f(1.0f), math::vec3f{ 0.0f, 0.0f, cameraZ });
	viewMatrix = math::inverse(cameraModelMatrix);
}

void render() {
	transform();
	shader->mModelMatrix = modelMatrix;
	shader->mViewMatrix = viewMatrix;
	shader->mProjectionMatrix = perspectiveMatrix;
	shader->mDiffuseTexture = texture;

	sgl->clear();
	sgl->useProgram(shader);

	sgl->bindVertexArray(vao);
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
	sgl->drawElement(DRAW_TRIANGLES, 0, 3);

}

void prepare() {
	shader = new TextureShader();

	// load texture
	image = Image::createImage("assets/textures/goku.jpg");
	texture = sgl->genTexture();
	sgl->bindTexture(texture);
	sgl->texImage2D(image->mWidth, image->mHeight, image->mData);
	sgl->texParameter(TEXTURE_FILTER, TEXTURE_FILTER_LINEAR);
	sgl->texParameter(TEXTURE_WRAP_U, TEXTURE_WRAP_REPEAT);
	sgl->texParameter(TEXTURE_WRAP_V, TEXTURE_WRAP_REPEAT);
	sgl->bindTexture(0);

	perspectiveMatrix = math::perspective(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

	auto cameraModelMatrix = math::translate(math::mat4f(1.0f), math::vec3f{ 0.0f, 0.0f, cameraZ });
	viewMatrix = math::inverse(cameraModelMatrix);

	sgl->enable(CULL_FACE);
	sgl->frontFace(FRONT_FACE_CCW);
	sgl->cullFace(BACK_FACE);

	float positions[] = {
		-0.5f, 0.0f, 0.0f,
		0.5f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f,
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

	// ebo
	ebo = sgl->genBuffer();
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
	sgl->bufferData(ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 3, indices);
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, 0);

	// vao
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

int main(int argc, char** argv)
{
	if (!app->initApplication(WIDTH, HEIGHT)) {
		return -1;
	}

	// Initialize GPU surface (GPU allocates its own framebuffer)
	sgl->initSurface(app->getWidth(), app->getHeight(), nullptr);

	prepare();

	// Create Slint UI
	auto ui = AppWindow::create();

	// Shared pixel buffer for transferring framebuffer to Slint
	// Our renderer outputs bottom-up (OpenGL style), Slint expects top-down
	slint::SharedPixelBuffer<slint::Rgba8Pixel> pixelBuffer(WIDTH, HEIGHT);

	slint::Timer timer;
	timer.start(slint::TimerMode::Repeated, std::chrono::milliseconds(16), [&]() {
		render();

		// Flip Y axis and copy from GPU framebuffer to Slint pixel buffer
		RGBA* src = sgl->getColorBuffer();
		if (src) {
			for (uint32_t y = 0; y < HEIGHT; ++y) {
				uint32_t srcRow = HEIGHT - 1 - y;
				memcpy(
					pixelBuffer.begin() + y * WIDTH,
					src + srcRow * WIDTH,
					WIDTH * sizeof(RGBA)
				);
			}
		}

		slint::Image image(pixelBuffer);
		ui->set_framebuffer(image);
	});

	ui->show();
	slint::run_event_loop();

	delete shader;
	Image::destroyImage(image);
	sgl->deleteTexture(texture);

	return 0;
}
