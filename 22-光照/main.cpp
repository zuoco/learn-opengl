#include <iostream>
#include <chrono>
#include "application/application.h"
#include "gpu/gpu.h"
#include "gpu/dataStructures.h"
#include "gpu/shader/lightShader.h"
#include "application/image.h"
#include "application/camera.h"
#include "appwindow.h"
#include "slint.h"

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

Camera* camera = nullptr;

// 顶点数据对应的vbo
uint32_t interleavedVbo = 0;

// 立方体的indices
uint32_t ebo = 0;

// 顶点属性专用vao
uint32_t vao = 0;

// 使用的Shader
LightShader* shader = nullptr;
DirectionalLight directionalLight;
math::vec3f envLight;

// 使用的texture
uint32_t texture = 0;
Image* image = nullptr;

// mvp变换矩阵
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

slint::Image createSlintImageFromFrameBuffer(FrameBuffer* fb) {
	uint32_t pixelCount = fb->mWidth * fb->mHeight;
	slint::SharedPixelBuffer<slint::Rgba8Pixel> pixelBuffer(fb->mWidth, fb->mHeight);

	RGBA* src = fb->mColorBuffer;
	slint::Rgba8Pixel* dst = pixelBuffer.begin();

	for (uint32_t i = 0; i < pixelCount; ++i) {
		dst[i].r = src[i].mR;
		dst[i].g = src[i].mG;
		dst[i].b = src[i].mB;
		dst[i].a = src[i].mA;
	}

	return slint::Image(pixelBuffer);
}

void prepare() {
	camera = new Camera(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f, { 0.0f, 1.0f, 0.0f });
	app->setCamera(camera);
	camera->setSpeed(0.02f);

	shader = new LightShader();
	directionalLight.color = {1.0f, 1.0f, 1.0f};
	directionalLight.direction = {-1.0f, -0.3f, -0.7f};
	envLight = {0.1f, 0.1f, 0.1f};

	// ��������
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

	// 创建indices对应ebo
	ebo = sgl->genBuffer();
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
	sgl->bufferData(ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 36, indices);
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, 0);

	// 总的交错式vbo
	interleavedVbo = sgl->genBuffer();
	sgl->bindBuffer(ARRAY_BUFFER, interleavedVbo);
	sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 288, vertices);

	// 创建vao，绑定
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

int main(int argc, char** argv) {
	app->initApplication(WIDTH, HEIGHT);

	// 将bmp指向内存关联到sgl渲染
	sgl->initSurface(app->getWidth(), app->getHeight());

	prepare();

	auto ui = AppWindow::create();

	// 设置输入回调
	ui->on_mouse_pressed([](int x, int y) {
		app->onRMouseDown(x, y);
	});
	ui->on_mouse_released([](int x, int y) {
		app->onRMouseUp(x, y);
	});
	ui->on_mouse_moved([](int x, int y) {
		app->onMouseMove(x, y);
	});
	ui->on_key_pressed([](slint::SharedString text) {
		if (text.size() > 0) {
			app->onKeyDown(text.data()[0]);
		}
	});
	ui->on_key_released([](slint::SharedString text) {
		if (text.size() > 0) {
			app->onKeyUp(text.data()[0]);
		}
	});

	// 渲染定时器，约60fps
	slint::Timer renderTimer(std::chrono::milliseconds(16), [&ui]() {
		camera->update();
		render();

		FrameBuffer* fb = sgl->getFrameBuffer();
		slint::Image frameImage = createSlintImageFromFrameBuffer(fb);
		ui->set_frame_image(frameImage);
	});

	ui->run();

	delete shader;
	Image::destroyImage(image);
	sgl->deleteTexture(texture);
	delete camera;

	return 0;
}
