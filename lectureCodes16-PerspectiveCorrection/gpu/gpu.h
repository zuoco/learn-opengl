#pragma once
#include "../global/base.h"
#include "frameBuffer.h"
#include "../application/application.h"
#include "../math/math.h"
#include "dataStructures.h"
#include "bufferObject.h"
#include "vao.h"
#include "shader.h"

#define sgl GPU::getInstance()

/*
* class GPU：
* 模拟GPU的绘图行为以及算法等
*/
class GPU {
public:
	static GPU* getInstance();
	GPU();

	~GPU();

	//接受外界传入的bmp对应的内存指针以及窗体的宽/高
	void initSurface(const uint32_t& width, const uint32_t& height, void* buffer = nullptr);

	//清除画布内容
	void clear();

	//打印状态机
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
	void perspectiveRecover(VsOutput& vsOutput);
	void screenMapping(VsOutput& vsOutput);

	void trim(VsOutput& vsOutput);

private:
	static GPU* mInstance;
	FrameBuffer* mFrameBuffer{ nullptr };

	//VBO相关/EBO也存在内部
	uint32_t mCurrentVBO{ 0 };
	uint32_t mCurrentEBO{ 0 };
	uint32_t mBufferCounter{ 0 };
	std::map<uint32_t, BufferObject*> mBufferMap;

	//VAO相关
	uint32_t mCurrentVAO{ 0 };
	uint32_t mVaoCounter{ 0 };
	std::map<uint32_t, VertexArrayObject*> mVaoMap;

	Shader* mShader{ nullptr };
	math::mat4f mScreenMatrix;
};