#include <iostream>
#include <chrono>
#include "application/application.h"
#include "gpu/gpu.h"
#include "gpu/dataStructures.h"
#include "gpu/shader/defaultShader.h"
#include "slint/slint.h"
#include "app.h"

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
    angle += 0.003f;
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
}

slint::Image createImageFromFrameBuffer(FrameBuffer* frameBuffer) {
    slint::SharedPixelBuffer<slint::Rgba8Pixel> buffer(
        static_cast<int>(frameBuffer->mWidth),
        static_cast<int>(frameBuffer->mHeight)
    );

    RGBA* src = frameBuffer->mColorBuffer;
    slint::Rgba8Pixel* dst = buffer.begin();

    for (uint32_t y = 0; y < frameBuffer->mHeight; ++y) {
        for (uint32_t x = 0; x < frameBuffer->mWidth; ++x) {
            uint32_t srcIdx = y * frameBuffer->mWidth + x;
            uint32_t dstIdx = (frameBuffer->mHeight - 1 - y) * frameBuffer->mWidth + x;
            dst[dstIdx].r = src[srcIdx].mR;
            dst[dstIdx].g = src[srcIdx].mG;
            dst[dstIdx].b = src[srcIdx].mB;
            dst[dstIdx].a = src[srcIdx].mA;
        }
    }

    return slint::Image(buffer);
}

int main() {
    sgl->initSurface(WIDTH, HEIGHT, nullptr);
    prepare();

    auto appWindow = AppWindow::create();
    appWindow->show();

    slint::Timer timer;
    timer.start(slint::TimerMode::Repeated, std::chrono::milliseconds(16), [&]() {
        render();
        auto image = createImageFromFrameBuffer(sgl->getFrameBuffer());
        appWindow->set_render_image(image);
    });

    slint::run_event_loop();

    delete shader;
    return 0;
}
