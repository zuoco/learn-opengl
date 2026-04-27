#pragma once
#include "../global/base.h"
#include "frameBuffer.h"
// Application layer removed, no longer needed
#include "../math/math.h"
#include "dataStructures.h"
#include "bufferObject.h"
#include "vao.h"
#include "shader.h"

#define sgl GPU::getInstance()

/*
* class GPU��
* ģ��GPU�Ļ�ͼ��Ϊ�Լ��㷨��
*/
class GPU {
public:
	static GPU* getInstance();
	GPU();

	~GPU();

	//������紫���bmp��Ӧ���ڴ�ָ���Լ�����Ŀ�/��
	void initSurface(const uint32_t& width, const uint32_t& height, void* buffer = nullptr);

	//�����������
	void clear();

	//��ӡ״̬��
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

	void enable(const uint32_t& value);

	void disable(const uint32_t& value);

	//cull face
	void frontFace(const uint32_t& value);

	void cullFace(const uint32_t& value);

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

	//VBO���/EBOҲ�����ڲ�
	uint32_t mCurrentVBO{ 0 };
	uint32_t mCurrentEBO{ 0 };
	uint32_t mBufferCounter{ 0 };
	std::map<uint32_t, BufferObject*> mBufferMap;

	//VAO���
	uint32_t mCurrentVAO{ 0 };
	uint32_t mVaoCounter{ 0 };
	std::map<uint32_t, VertexArrayObject*> mVaoMap;

	Shader* mShader{ nullptr };

public:
	FrameBuffer* getFrameBuffer() const { return mFrameBuffer; }
	math::mat4f mScreenMatrix;

	//cull face
	bool mEnableCullFace{ true };
	uint32_t mFrontFace{ FRONT_FACE_CCW };
	uint32_t mCullFace{ BACK_FACE };
};