#pragma once
#include "../global/base.h"

/*
* class Raster
* 软件光栅化： 通过数学运算在CPU上模拟GPU的光栅化过程。
* 对外提供静态函数接口，传入离散的图元点，返回光栅化后的像素数组
*/
class Raster {
public:
	Raster();
	~Raster();

	static void rasterizeLine(
		std::vector<Point>& results,
		const Point& v0,
		const Point& v1
	);

	static void interpolantLine(const Point& v0, const Point& v1, Point& target);

	// 三角形光栅化
	// 扫描线填充算法
	// 	边界框计算：找出包含三角形的最小矩形区域
	// 逐像素检测：遍历边界框内每个像素点
	// 叉积测试：使用向量叉积判断点是否在三角形内部，如果所有叉积符号相同（全正或全负），则点在三角形内
	static void rasterizeTriangle(
		std::vector<Point>& results,
		const Point& v0,
		const Point& v1,
		const Point& v2
	);

	// 颜色插值函数
	// 使用面积比例作为权重
	static void interpolantTriangle(const Point& v0, const Point& v1, const Point& v2, Point& p);
};