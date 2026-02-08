#include "gpu.h"
#include "raster.h"
#include "clipper.h"

GPU* GPU::mInstance = nullptr;
GPU* GPU::getInstance() {
	if (!mInstance) {
		mInstance = new GPU();
	}

	return mInstance;
}

GPU::GPU() {}

GPU::~GPU() {
	if (mFrameBuffer) {
		delete mFrameBuffer;
	}

	for (auto iter : mBufferMap) {
		delete iter.second;
	}
	mBufferMap.clear();

	for (auto iter : mVaoMap) {
		delete iter.second;
	}
	mVaoMap.clear();
}

void GPU::initSurface(const uint32_t& width, const uint32_t& height, void* buffer) {
	mFrameBuffer = new FrameBuffer(width, height, buffer);
	mScreenMatrix = math::screenMatrix<float>(width - 1, height - 1);
}

void GPU::clear() {
	size_t pixelSize = mFrameBuffer->mWidth * mFrameBuffer->mHeight;
	std::fill_n(mFrameBuffer->mColorBuffer, pixelSize, RGBA(0, 0, 0, 0));
}

void GPU::printVAO(const uint32_t& vaoID) {
	auto iter = mVaoMap.find(vaoID);
	if (iter != mVaoMap.end()) {
		iter->second->print();
	}
}

uint32_t GPU::genBuffer() {
	mBufferCounter++;
	mBufferMap.insert(std::make_pair(mBufferCounter, new BufferObject()));

	return mBufferCounter;
}

void GPU::deleteBuffer(const uint32_t& bufferID) {
	auto iter = mBufferMap.find(bufferID);
	if (iter != mBufferMap.end()) {
		delete iter->second;
	}
	else {
		return;
	}

	mBufferMap.erase(iter);
}

void GPU::bindBuffer(const uint32_t& bufferType, const uint32_t& bufferID) {
	if (bufferType == ARRAY_BUFFER) {
		mCurrentVBO = bufferID;
	}
	else if (bufferType == ELEMENT_ARRAY_BUFFER) {
		mCurrentEBO = bufferID;
	}
}

void GPU::bufferData(const uint32_t& bufferType, size_t dataSize, void* data) {
	uint32_t bufferID = 0;
	if (bufferType == ARRAY_BUFFER) {
		bufferID = mCurrentVBO;
	}
	else if (bufferType == ELEMENT_ARRAY_BUFFER) {
		bufferID = mCurrentEBO;
	}
	else {
		assert(false);
	}

	auto iter = mBufferMap.find(bufferID);
	if (iter == mBufferMap.end()) {
		assert(false);
	}

	BufferObject* bufferObject = iter->second;
	bufferObject->setBufferData(dataSize, data);
}

uint32_t GPU::genVertexArray() {
	mVaoCounter++;
	mVaoMap.insert(std::make_pair(mVaoCounter, new VertexArrayObject()));

	return mVaoCounter;
}

void GPU::deleteVertexArray(const uint32_t& vaoID) {
	auto iter = mVaoMap.find(vaoID);
	if (iter != mVaoMap.end()) {
		delete iter->second;
	}
	else {
		return;
	}

	mVaoMap.erase(iter);
}

void GPU::bindVertexArray(const uint32_t& vaoID) {
	mCurrentVAO = vaoID;
}

void GPU::vertexAttributePointer(
	const uint32_t& binding,
	const uint32_t& itemSize,
	const uint32_t& stride,
	const uint32_t& offset)
{
	auto iter = mVaoMap.find(mCurrentVAO);
	if (iter == mVaoMap.end()) {
		assert(false);
	}

	auto vao = iter->second;
	vao->set(binding, mCurrentVBO, itemSize, stride, offset);
}

void GPU::useProgram(Shader* shader) {
	mShader = shader;
}

void GPU::drawElement(const uint32_t& drawMode, const uint32_t& first, const uint32_t& count) {
	if (mCurrentVAO == 0 || mShader == nullptr || count == 0) {
		return;
	}

	//1 get vao
	auto vaoIter = mVaoMap.find(mCurrentVAO);
	if (vaoIter == mVaoMap.end()) {
		std::cerr << "Error: current vao is invalid!" << std::endl;
		return;
	}

	const VertexArrayObject* vao = vaoIter->second;
	auto bindingMap = vao->getBindingMap();

	//2 get ebo
	auto eboIter = mBufferMap.find(mCurrentEBO);
	if (eboIter == mBufferMap.end()) {
		std::cerr << "Error: current ebo is invalid!" << std::endl;
		return;
	}

	const BufferObject* ebo = eboIter->second;

	/*
	* VertexShader处理阶段
	* 作用：
	*	按照输入的Ebo的index顺序来处理顶点，依次通过vsShader，
		得到的输出结果按序放入vsOutputs中
	*/
	std::vector<VsOutput> vsOutputs{};
	vertexShaderStage(vsOutputs, vao, ebo, first, count);

	if (vsOutputs.empty()) return;

	/*
	* Clip Space处理阶段
	* 作用：
	*	在剪裁空间，对所有输出的图元进行剪裁拼接等
	*/
	std::vector<VsOutput> clipOutputs{};
	Clipper::doClipSpace(drawMode, vsOutputs, clipOutputs);
	if (clipOutputs.empty()) return;

	vsOutputs.clear();

	/*
	* NDC处理阶段
	* 作用：
	*	将顶点转化到NDC下
	*/
	for (auto& output : clipOutputs) {
		perspectiveDivision(output);
	}

	/*
	* 屏幕映射处理阶段
	* 作用：
	*	将NDC下的点，通过screenMatrix，转化到屏幕空间
	*/
	for (auto& output : clipOutputs) {
		screenMapping(output);
	}

	/*
	* 光栅化处理阶段
	* 作用：
	*	离散出所有需要的Fragment
	*/
	std::vector<VsOutput> rasterOutputs;
	Raster::rasterize(rasterOutputs, drawMode, clipOutputs);


	if (rasterOutputs.empty()) return;

	/*
	* 颜色输出处理阶段
	* 作用：
	*	 将颜色进行输出
	*/
	FsOutput fsOutput;
	uint32_t pixelPos = 0;
	for (uint32_t i = 0; i < rasterOutputs.size(); ++i) {
		mShader->fragmentShader(rasterOutputs[i], fsOutput);
		pixelPos = fsOutput.mPixelPos.y * mFrameBuffer->mWidth + fsOutput.mPixelPos.x;
		mFrameBuffer->mColorBuffer[pixelPos] = fsOutput.mColor;
	}
}

void GPU::vertexShaderStage(
	std::vector<VsOutput>& vsOutputs,
	const VertexArrayObject* vao,
	const BufferObject* ebo,
	const uint32_t first,
	const uint32_t count) {
	auto bindingMap = vao->getBindingMap();
	byte* indicesData = ebo->getBuffer();

	uint32_t index = 0;
	for (uint32_t i = first; i < first + count; ++i) {
		//获取Ebo中第i个index
		size_t indicesOffset = i * sizeof(uint32_t);
		memcpy(&index, indicesData + indicesOffset, sizeof(uint32_t));

		VsOutput output = mShader->vertexShader(bindingMap, mBufferMap, index);
		vsOutputs.push_back(output);
	}
}

void GPU::perspectiveDivision(VsOutput& vsOutput) {
	float oneOverW = 1.0f / vsOutput.mPosition.w;

	vsOutput.mPosition *= oneOverW;
	vsOutput.mPosition.w = 1.0f;

	////修剪毛刺
	//if (vsOutput.mPosition.x < -1.0f) {
	//	vsOutput.mPosition.x = -1.0f;
	//}

	//if (vsOutput.mPosition.x > 1.0f) {
	//	vsOutput.mPosition.x = 1.0f;
	//}

	//if (vsOutput.mPosition.y < -1.0f) {
	//	vsOutput.mPosition.y = -1.0f;
	//}

	//if (vsOutput.mPosition.y > 1.0f) {
	//	vsOutput.mPosition.y = 1.0f;
	//}

	//if (vsOutput.mPosition.z < -1.0f) {
	//	vsOutput.mPosition.z = -1.0f;
	//}

	//if (vsOutput.mPosition.z > 1.0f) {
	//	vsOutput.mPosition.z = 1.0f;
	//}
}

void GPU::screenMapping(VsOutput& vsOutput) {
	vsOutput.mPosition = mScreenMatrix * vsOutput.mPosition;
}
