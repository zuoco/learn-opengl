#pragma once
#include "../global/base.h"
#include "frameBuffer.h"
#include "../math/math.h"
#include "dataStructures.h"
#include "bufferObject.h"
#include "vao.h"
#include "shader.h"

#define sgl GPU::getInstance()

/*
* class GPU：
* 模拟GPU的绘图，为自己算法服务
*/
class GPU {
public:
	static GPU* getInstance();
	GPU();

	~GPU();

	//初始化渲染表面，传入宽/高，内部自行分配颜色缓冲区
	void initSurface(const uint32_t& width, const uint32_t& height);

	uint32_t getWidth() const { return mFrameBuffer ? mFrameBuffer->mWidth : 0; }
	uint32_t getHeight() const { return mFrameBuffer ? mFrameBuffer->mHeight : 0; }
	RGBA* getColorBuffer() const { return mFrameBuffer ? mFrameBuffer->mColorBuffer : nullptr; }

	//清理颜色缓冲区
	void clear();

	//打印状态：
	void printVAO(const uint32_t& vaoID);

	uint32_t genBuffer();
	void deleteBuffer(const uint32_t& bufferID);
	void bindBuffer(const uint32_t& bufferType, const uint32_t& bufferID);
	void bufferData(const uint32_t& bufferType, size_t dataSize, void* data);

	uint32_t genVertexArray();
	void deleteVertexArray(const uint32_t& vaoID);
	void bindVertexArray(const uint32_t& vaoID);
	void vertexAttributePointer(
		const uint32_t& binding,
		const uint32_t& itemSize,
		const uint32_t& stride,
		const uint32_t& offset);

	void useProgram(Shader* shader);

	void drawElement(const uint32_t& drawMode, const uint32_t& first, const uint32_t& count);

private:
	void vertexShaderStage(
		std::vector<VsOutput>& vsOutputs,
		const VertexArrayObject* vao,
		const BufferObject* ebo,
		const uint32_t first,
		const uint32_t count);

	void perspectiveDivision(VsOutput& vsOutput);
	void screenMapping(VsOutput& vsOutput);

private:
	static GPU* mInstance;
	FrameBuffer* mFrameBuffer{ nullptr };

	//VBO集合/EBO也包含在内
	uint32_t mCurrentVBO{ 0 };
	uint32_t mCurrentEBO{ 0 };
	uint32_t mBufferCounter{ 0 };
	std::map<uint32_t, BufferObject*> mBufferMap;

	//VAO集合
	uint32_t mCurrentVAO{ 0 };
	uint32_t mVaoCounter{ 0 };
	std::map<uint32_t, VertexArrayObject*> mVaoMap;

	Shader* mShader{ nullptr };
	math::mat4f mScreenMatrix;
};
