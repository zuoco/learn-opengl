#include "raster.h"
#include "../math/math.h"

Raster::Raster() {}

Raster::~Raster() {}

void Raster::rasterizeLine(
	std::vector<Point>& results,
	const Point& v0,
	const Point& v1) {

	Point start = v0;
	Point end = v1;

	//1 保证x方向是从小到大的
	if (start.x > end.x) {
		auto tmp = start;
		start = end;
		end = tmp;
	}

	results.push_back(start);

	//2 保证y方向也是从小到大，如果需要翻转，必须记录
	bool flipY = false;
	if (start.y > end.y) {
		start.y *= -1.0f;
		end.y *= -1.0f;
		flipY = true;
	}

	//3 保证斜率在0-1之间，如果需要调整，必须记录
	int deltaX = static_cast<int>(end.x - start.x);
	int deltaY = static_cast<int>(end.y - start.y);

	bool swapXY = false;
	if (deltaX < deltaY) {
		std::swap(start.x, start.y);
		std::swap(end.x, end.y);
		std::swap(deltaX, deltaY);
		swapXY = true;
	}

	//4 brensenham
	int currentX = static_cast<int>(start.x);
	int currentY = static_cast<int>(start.y);

	int resultX = 0;
	int resultY = 0;

	Point currentPoint;
	int p = 2 * deltaY - deltaX;

	for (int i = 0; i < deltaX; ++i) {
		if (p >= 0) {
			currentY += 1;
			p -= 2 * deltaX;
		}

		currentX += 1;
		p += 2 * deltaY;

		//处理新xy，flip and swap

		resultX = currentX;
		resultY = currentY;
		if (swapXY) {
			std::swap(resultX, resultY);
		}

		if (flipY) {
			resultY *= -1;
		}

		//产生新顶点
		currentPoint.x = resultX;
		currentPoint.y = resultY;

		interpolantLine(start, end, currentPoint);

		results.push_back(currentPoint);
	}

}

void Raster::interpolantLine(const Point& v0, const Point& v1, Point& target) {
	float weight = 1.0f;
	if (v1.x != v0.x) {
		//用x做比例
		weight = (float)(target.x - v0.x) / (float)(v1.x - v0.x);
	}else if (v1.y != v0.y) {
		//用y做比例
		weight = (float)(target.y - v0.y) / (float)(v1.y - v0.y);
	}

	RGBA result;
	result.mR = static_cast<byte>(static_cast<float>(v1.color.mR) * weight + (1.0f - weight) * static_cast<float>(v0.color.mR));
	result.mG = static_cast<byte>(static_cast<float>(v1.color.mG) * weight + (1.0f - weight) * static_cast<float>(v0.color.mG));
	result.mB = static_cast<byte>(static_cast<float>(v1.color.mB) * weight + (1.0f - weight) * static_cast<float>(v0.color.mB));
	result.mA = static_cast<byte>(static_cast<float>(v1.color.mA) * weight + (1.0f - weight) * static_cast<float>(v0.color.mA));

	target.color = result;
}

void Raster::rasterizeTriangle(std::vector<Point>& results, const Point& v0, const Point& v1, const Point& v2) {
	// 计算包围盒，包含三角形的最小矩阵
	int maxX = static_cast<int>(std::max(v0.x, std::max(v1.x, v2.x)));
	int minX = static_cast<int>(std::min(v0.x, std::min(v1.x, v2.x)));
	int maxY = static_cast<int>(std::max(v0.y, std::max(v1.y, v2.y)));
	int minY = static_cast<int>(std::min(v0.y, std::min(v1.y, v2.y)));

	math::vec2f pv0, pv1, pv2;
	Point result;
	// 遍历包围盒内的每个像素点，i为x坐标，j为y坐标
	// 找到所有位于三角形内部的点
	for (int i = minX; i <= maxX; ++i) {
		for (int j = minY; j <= maxY; ++j) {

			// 计算当前像素点到三个顶点的向量（从像素指向顶点）
			pv0 = math::vec2f(v0.x - i, v0.y - j);
			pv1 = math::vec2f(v1.x - i, v1.y - j);
			pv2 = math::vec2f(v2.x - i, v2.y - j);

			// 根据二维向量的叉积以及右手定则可以确定向量a旋转到b是顺时针还是逆时针
			auto cross1 = math::cross(pv0, pv1);
			auto cross2 = math::cross(pv1, pv2);
			auto cross3 = math::cross(pv2, pv0);

			bool negativeAll = cross1 < 0 && cross2 < 0 && cross3 < 0;
			bool positiveAll = cross1 > 0 && cross2 > 0 && cross3 > 0;

			// 只有当叉乘结果的符号一致时，点才在三角形内部
			if (negativeAll || positiveAll) {
				result.x = i;
				result.y = j;
				// 通过插值求解该点的颜色
				interpolantTriangle(v0, v1, v2, result);

				// 该点在三角形内，保存起来
				results.push_back(result);
			}
		}
	}
}

void Raster::interpolantTriangle(const Point& v0, const Point& v1, const Point& v2, Point& p) {
	// 点p到 v0,v1,v2 就是三个向量
	// 这三个向量将三角形划分成三个三角形
	// 三个三角形面积占整个大三角形面积的比例作为权重
	// 通过叉乘可以求出两个向量围成的四边形面积
	// 我们这里直接使用平行四边形的面积来计算权重比例。
	auto e1 = math::vec2f(v1.x - v0.x, v1.y - v0.y);
	auto e2 = math::vec2f(v2.x - v0.x, v2.y - v0.y);
	float sumArea = std::abs(math::cross(e1, e2));

	auto pv0 = math::vec2f(v0.x - p.x, v0.y - p.y);
	auto pv1 = math::vec2f(v1.x - p.x, v1.y - p.y);
	auto pv2 = math::vec2f(v2.x - p.x, v2.y - p.y);

	float v0Area = std::abs(math::cross(pv1, pv2));
	float v1Area = std::abs(math::cross(pv0, pv2));
	float v2Area = std::abs(math::cross(pv0, pv1));

	// 计算权重，使用面积比例作为权重
	float weight0 = v0Area / sumArea;
	float weight1 = v1Area / sumArea;
	float weight2 = v2Area / sumArea;

	RGBA result;
	auto c0 = v0.color;
	auto c1 = v1.color;
	auto c2 = v2.color;

	result.mR = static_cast<float>(c0.mR) * weight0 + static_cast<float>(c1.mR) * weight1 + static_cast<float>(c2.mR) * weight2;
	result.mG = static_cast<float>(c0.mG) * weight0 + static_cast<float>(c1.mG) * weight1 + static_cast<float>(c2.mG) * weight2;
	result.mB = static_cast<float>(c0.mB) * weight0 + static_cast<float>(c1.mB) * weight1 + static_cast<float>(c2.mB) * weight2;
	result.mA = static_cast<float>(c0.mA) * weight0 + static_cast<float>(c1.mA) * weight1 + static_cast<float>(c2.mA) * weight2;

	p.color = result;
}
