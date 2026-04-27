#include "clipper.h"
#include "../math/math.h"

void Clipper::doClipSpace(const uint32_t& drawMode, const std::vector<VsOutput>& primitives, std::vector<VsOutput>& outputs) {
	//保证输出为空
	outputs.clear();

	if (drawMode == DRAW_TRIANGLES) {
		std::vector<VsOutput> primitive;
		std::vector<VsOutput> results;

		for (uint32_t i = 0; i < primitives.size(); i += 3) {
			primitive.clear();
			results.clear();
			auto start = primitives.begin() + i;
			auto end = primitives.begin() + i + 3;
			primitive.assign(start, end);

			Clipper::sutherlandHodgman(drawMode, primitive, results);

			if (results.empty()) {
				continue;
			}

			//进行三角形缝补
			for (uint32_t c = 0; c < results.size() - 2; ++c) {
				outputs.push_back(results[0]);
				outputs.push_back(results[c + 1]);
				outputs.push_back(results[c + 2]);
			}
		}
	}
	else if (drawMode == DRAW_LINES) {
		std::vector<VsOutput> primitive;
		std::vector<VsOutput> results;

		for (uint32_t i = 0; i < primitives.size(); i += 2) {
			primitive.clear();
			results.clear();
			auto start = primitives.begin() + i;
			auto end = primitives.begin() + i + 2;
			primitive.assign(start, end);
			Clipper::sutherlandHodgman(drawMode, primitive, results);

			outputs.push_back(results[0]);
			outputs.push_back(results[1]);
		}
	}
}


void Clipper::sutherlandHodgman(const uint32_t& drawMode, const std::vector<VsOutput>& primitive, std::vector<VsOutput>& outputs) {
	assert(outputs.size() == 0);

	std::vector<math::vec4f> clipPlanes = {
		//judge w > 0
		math::vec4f(0.0f, 0.0f, 0.0f, 1.0f),
		//near
		math::vec4f(0.0f, 0.0f, 1.0f, 1.0f),
		//far
		math::vec4f(0.0f, 0.0f, -1.0f, 1.0f),
		//left
		math::vec4f(1.0f, 0.0f, 0.0f, 1.0f),
		//right
		math::vec4f(-1.0f, 0.0f, 0.0f, 1.0f),
		//top
		math::vec4f(0.0f, -1.0f, 0.0f, 1.0f),
		//bottom
		math::vec4f(0.0f, 1.0f, 0.0f, 1.0f)
	};

	//Sutherland-Hodgman algorithm
	outputs = primitive;
	std::vector<VsOutput> inputs;

	//遍历每一个平面进行检查
	for (uint32_t i = 0; i < clipPlanes.size(); ++i) {
		//用上一次的输出作为输入点集
		inputs = outputs;
		outputs.clear();

		//遍历输入的每一个点
		for (uint32_t p = 0; p < inputs.size(); ++p) {
			//取出点P
			auto current = inputs[p];
			//取出点S
			auto last = inputs[(p + inputs.size() - 1) % inputs.size()];

			//1 判定P点在内部,即3/4情况
			if (inside(current.mPosition, clipPlanes[i])) {
				//这里加了限制因素，线条如果到了最后一个顶点，不准回头计算交点
				if (drawMode != DRAW_LINES || p != inputs.size() - 1) {

					//2 判定S点不在内部，情况3
					if (!inside(last.mPosition, clipPlanes[i])) {
						//求交点I
						auto intersectPoint = intersect(last, current, clipPlanes[i]);

						//按照情况3输出I
						outputs.push_back(intersectPoint);
					}
				}
				
				//P点只要在内部，都会输出（情况3/4）
				outputs.push_back(current);
			}
			//P点不在内部，即1/2情况
			else {
				// 这里加了限制因素，线条如果到了最后一个顶点，不准回头计算交点
				if (drawMode != DRAW_LINES || p != inputs.size() - 1) {

					//S点在内部,情况2,输出交点I
					if (inside(last.mPosition, clipPlanes[i])) {
						auto intersectPoint = intersect(last, current, clipPlanes[i]);
						outputs.push_back(intersectPoint);
					}
				}

				//S点不在内部，情况1，无输出
				
			}
		}
	}
}

bool Clipper::inside(const math::vec4f& point, const math::vec4f& plane) {
	return math::dot(point, plane) >= 0.0f;
}

//注意，这里只能插值位于平面两侧的点
VsOutput Clipper::intersect(const VsOutput& last, const VsOutput& current, const math::vec4f& plane) {
	VsOutput output;

	float distanceLast = math::dot(last.mPosition, plane);
	float distanceCurrent = math::dot(current.mPosition, plane);
	float weight = distanceLast / (distanceLast - distanceCurrent);

	output.mPosition = math::lerp(last.mPosition, current.mPosition, weight);
	output.mColor = math::lerp(last.mColor, current.mColor, weight);
	output.mUV = math::lerp(last.mUV, current.mUV, weight);

	return output;
}
