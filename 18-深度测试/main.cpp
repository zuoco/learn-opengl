#include <chrono>
#include <cstring>
#include "slint.h"
#include "app.h"

#include "gpu/gpu.h"
#include "gpu/dataStructures.h"
#include "gpu/shader/defaultShader.h"

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

//���������Σ��������Զ�Ӧvbo
uint32_t positionVbo0 = 0;
uint32_t positionVbo1 = 0;
uint32_t colorVbo0 = 0;
uint32_t colorVbo1 = 0;

uint32_t uvVbo = 0;

//�����ε�indices
uint32_t ebo = 0;

//����������ר��vao
uint32_t vao0 = 0;
uint32_t vao1 = 0;

//ʹ�õ�Shader
DefaultShader* shader = nullptr;

//mvp�任����
math::mat4f modelMatrix;
math::mat4f viewMatrix;
math::mat4f perspectiveMatrix;

float angle = 0.0f;
float cameraZ = 2.0f;

void transform() {
	//angle += 0.01f;
	//cameraZ -= 0.01f;

	//ģ�ͱ任
	modelMatrix = math::rotate(math::mat4f(1.0f), angle, math::vec3f{ 0.0f, 1.0f, 0.0f });

	//��ͼ�任
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
	sgl->bindVertexArray(vao0);
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
	sgl->drawElement(DRAW_TRIANGLES, 0, 3);

	sgl->bindVertexArray(vao1);
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

	//��һ��������
	float positions0[] = {
		-0.5f, 0.0f, 0.0f,
		0.5f, 0.0f, 0.0f,
		0.25f, 0.5f, 0.0f,
	};

	float colors0[] = {
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
	};

	//�ڶ���������
	float positions1[] = {
		0.3f, 0.0f, -0.3f,
		0.8f, 0.0f, -0.3f,
		0.45f, 0.5f, -0.3f,
	};

	float colors1[] = {
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
	};

	float uvs[] = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
	};

	uint32_t indices[] = { 0, 1, 2 };

	//����indices��Ӧebo, ���߹���
	ebo = sgl->genBuffer();
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
	sgl->bufferData(ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 3, indices);
	sgl->bindBuffer(ELEMENT_ARRAY_BUFFER, 0);

	//����uv��Ӧ��vbo�����߹���
	uvVbo = sgl->genBuffer();
	sgl->bindBuffer(ARRAY_BUFFER, uvVbo);
	sgl->bufferData(ARRAY_BUFFER, sizeof(float) * 6, uvs);
	sgl->bindBuffer(ARRAY_BUFFER, 0);

	//����vao���Ұ
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


	//����vao���Ұ
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
}

int main() {
	auto ui = AppWindow::create();

	//��ʼ��GPU���棬�����ⲿbuffer
	sgl->initSurface(WIDTH, HEIGHT);

	prepare();

	//ʹ�ö�ʱ��������Ⱦѭ��
	slint::Timer renderTimer(std::chrono::milliseconds(16), [&]() {
		render();

		FrameBuffer* fb = sgl->getFrameBuffer();
		slint::SharedPixelBuffer<slint::Rgba8Pixel> pixelBuffer(
			WIDTH, HEIGHT,
			reinterpret_cast<slint::Rgba8Pixel*>(fb->mColorBuffer)
		);

		slint::Image image(pixelBuffer);
		ui->set_rendered_image(image);
	});

	std::cout << "Starting Slint event loop..." << std::endl;
	slint::run_event_loop();
	std::cout << "Event loop exited." << std::endl;

	delete shader;
	return 0;
}
