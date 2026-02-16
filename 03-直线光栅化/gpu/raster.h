#pragma once
#include "../global/base.h"

/*
 * class Raster
 * 对外提供静态函数接口，传入离散的图元点，返回光栅化后的像素数组
 - __静态方法模式__：所有功能函数都是静态的，无需实例化即可使用
 * __工具类模式__：不维护状态，仅提供计算功能
 */
class Raster {
public:
    Raster();
    ~Raster();

    // 将一条直线光栅化为离散的像素点
    // 实现了Bresenham直线算法，将连续直线转换为离散像素点。
    static void rasterizeLine(
        std::vector<Point>& results,
        const Point& v0,
        const Point& v1
    );

    // 在直线上对某个属性进行插值计算
    // 这个函数实现了性插值（Linear Interpolation），用于计算直线上任意点的颜色值。
    static void interpolantLine(const Point& v0, const Point& v1, Point& target);
};
