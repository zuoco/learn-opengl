#include "shader.h"
#include <cstring>

math::vec4f Shader::getVector(
	const std::map<uint32_t, BindingDescription>& bindingMap,
	const std::map<uint32_t, BufferObject*>& bufferMap,
	const uint32_t& attributeLocation,
	const uint32_t& index) {

	//1 ȡ�������Ե�Description
	auto bindingIter = bindingMap.find(attributeLocation);
	if (bindingIter == bindingMap.end()) {
		assert(false);
	}

	auto bindingDescription = bindingIter->second;

	//2 ȡ�����������ڵ�vbo
	auto vboID = bindingDescription.mVboId;
	auto vboIter = bufferMap.find(vboID);
	if (vboIter == bufferMap.end()) {
		assert(false);
	}

	BufferObject* vbo = vboIter->second;

	//3 ����������vbo�е�ƫ����
	uint32_t dataOffset = bindingDescription.mStride * index + bindingDescription.mOffset;
	uint32_t dataSize = bindingDescription.mItemSize * sizeof(float);

	const byte* buffer = vbo->getBuffer() + dataOffset;

	math::vec4f result;

	//��������Ҫ�����ݣ����4��float
	memcpy(&result, buffer, dataSize);

	return result;
}
