#pragma once
#include "../global/base.h"
#include "dataStructures.h"
#include "bufferObject.h"
#include "texture.h"

class Shader {
public:
	Shader() {}
	~Shader() {}
	virtual VsOutput vertexShader(
		//VAO当中的bindingMap
		const std::map<uint32_t, BindingDescription>& bindingMap,

		//VBO当中的bindingMap
		const std::map<uint32_t,BufferObject*>& bufferMap,

		//当前要处理的顶点的index
		const uint32_t& index
	) = 0;

	virtual void fragmentShader(const VsOutput& input, FsOutput& output, const std::map<uint32_t, Texture*>& textures) = 0;

	//tool functions
public:
	//你要先看在VertexShader函数中如何使用，再看逻辑
	math::vec4f getVector(
		const std::map<uint32_t, BindingDescription>& bindingMap,
		const std::map<uint32_t, BufferObject*>& bufferMap,
		const uint32_t& attributeLocation,//当前属性的编号
		const uint32_t& index);//当前顶点编号

	RGBA vectorToRGBA(const math::vec4f& v) {
		RGBA color;
		color.mR = v.x * 255.0;
		color.mG = v.y * 255.0;
		color.mB = v.z * 255.0;
		color.mA = v.w * 255.0;

		return color;
	}
};
