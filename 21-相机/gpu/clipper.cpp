#include "clipper.h"
#include <cstdint>
#include "../math/math.h"

void Clipper::doClipSpace(const uint32_t& drawMode, const std::vector<VsOutput>& primitives, std::vector<VsOutput>& outputs) {
	//��֤���Ϊ��
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

			//���������η첹
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

	//����ÿһ��ƽ����м��
	for (uint32_t i = 0; i < clipPlanes.size(); ++i) {
		//����һ�ε������Ϊ����㼯
		inputs = outputs;
		outputs.clear();

		//���������ÿһ����
		for (uint32_t p = 0; p < inputs.size(); ++p) {
			//ȡ����P
			auto current = inputs[p];
			//ȡ����S
			auto last = inputs[(p + inputs.size() - 1) % inputs.size()];

			//1 �ж�P�����ڲ�,��3/4���
			if (inside(current.mPosition, clipPlanes[i])) {
				//��������������أ���������������һ�����㣬��׼��ͷ���㽻��
				if (drawMode != DRAW_LINES || p != inputs.size() - 1) {

					//2 �ж�S�㲻���ڲ������3
					if (!inside(last.mPosition, clipPlanes[i])) {
						//�󽻵�I
						auto intersectPoint = intersect(last, current, clipPlanes[i]);

						//�������3���I
						outputs.push_back(intersectPoint);
					}
				}
				
				//P��ֻҪ���ڲ���������������3/4��
				outputs.push_back(current);
			}
			//P�㲻���ڲ�����1/2���
			else {
				// ��������������أ���������������һ�����㣬��׼��ͷ���㽻��
				if (drawMode != DRAW_LINES || p != inputs.size() - 1) {

					//S�����ڲ�,���2,�������I
					if (inside(last.mPosition, clipPlanes[i])) {
						auto intersectPoint = intersect(last, current, clipPlanes[i]);
						outputs.push_back(intersectPoint);
					}
				}

				//S�㲻���ڲ������1�������
				
			}
		}
	}
}

bool Clipper::inside(const math::vec4f& point, const math::vec4f& plane) {
	return math::dot(point, plane) > 0.0f;
}

//ע�⣬����ֻ�ܲ�ֵλ��ƽ������ĵ�
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

bool Clipper::cullFace(
	const uint32_t& frontFace,
	const uint32_t& cullFace,
	const VsOutput& v0,
	const VsOutput& v1,
	const VsOutput& v2) {

	math::vec3f edge1 = v1.mPosition - v0.mPosition;
	math::vec3f edge2 = v2.mPosition - v0.mPosition;

	math::vec3f normal = math::cross(edge1, edge2);

	//ע�⣬��ʱNDC�����Ѿ�λ������������ϵ�£������Ҫ���������Ȼ�
	if (cullFace == BACK_FACE) {

		//����ʱ�������,ʹ������ʾ�⣬z>0Ϊfront������
		if (frontFace == FRONT_FACE_CCW) {
			return normal.z > 0;
		}
		else {
		//��˳ʱ�������,ʹ������ʾ�⣬z<0Ϊfront������
			return normal.z < 0;
		}
	}
	else {
		//����ʱ�������,ʹ������ʾ�⣬z<0Ϊback������
		if (frontFace == FRONT_FACE_CCW) {
			return normal.z < 0;
		}
		else {
		//��˳ʱ�������,ʹ������ʾ�⣬z>0Ϊback������
			return normal.z > 0;
		}
	}
}
