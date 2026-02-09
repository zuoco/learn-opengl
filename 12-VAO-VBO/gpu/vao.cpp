#include "vao.h"

VertexArrayObject::VertexArrayObject() {}
VertexArrayObject::~VertexArrayObject() {}

void VertexArrayObject::set(uint32_t binding, uint32_t vboId, size_t itemSize, size_t stride, size_t offset) {
	auto iter = mBindingMap.find(binding);
	if (iter == mBindingMap.end()) {
		iter = mBindingMap.insert(std::make_pair(binding, BindingDescription())).first;
	}

	auto& des = iter->second;
	des.mVboId = vboId;
	des.mItemSize = itemSize;
	des.mStride = stride;
	des.mOffset = offset;
}

std::map<uint32_t, BindingDescription> VertexArrayObject::getBindingMap() const {
	return mBindingMap;
}