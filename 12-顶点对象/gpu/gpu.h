#pragma once
#include "../global/base.h"
#include "frameBuffer.h"
#include "../math/math.h"
#include "dataStructures.h"
#include "bufferObject.h"
#include "vao.h"

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

	uint32_t genBuffer();
	void deleteBuffer(const uint32_t& bufferID);

	uint32_t genVertexArray();
	void deleteVertexArray(const uint32_t& vaoID);

	FrameBuffer* getFrameBuffer() const { return mFrameBuffer; }

private:
	static GPU* mInstance;
	FrameBuffer* mFrameBuffer{ nullptr };

	//VBO���/EBOҲ�����ڲ�
	uint32_t mBufferCounter{ 0 };
	std::map<uint32_t, BufferObject*> mBufferMap;

	//VAO���
	uint32_t mVaoCounter{ 0 };
	std::map<uint32_t, VertexArrayObject*> mVaoMap;
};
