#include "raster.h"
#include <algorithm>

Raster::Raster() {}

Raster::~Raster() {}

void Raster::rasterizeLine( std::vector<Point>& results, const Point& v0, const Point& v1) {

    Point start = v0;
    Point end = v1;

    // 1 保证x方向是从小到大的
    if (start.x > end.x) {
        auto tmp = start;
        start = end;
        end = tmp;
    }

    results.push_back(start);

    // 2 保证y方向也是从小到大，如果需要翻转，必须记录
    bool flipY = false;
    if (start.y > end.y) {
        start.y *= -1;
        end.y *= -1;
        flipY = true;
    }

    // 3 保证斜率在0-1之间，如果需要调整，必须记录
    int deltaX = static_cast<int>(end.x - start.x);
    int deltaY = static_cast<int>(end.y - start.y);

    bool swapXY = false;
    if (deltaX < deltaY) {
        std::swap(start.x, start.y);
        std::swap(end.x, end.y);
        std::swap(deltaX, deltaY);
        swapXY = true;
    }

    // 4 brensenham
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

        // 处理新xy，flip and swap
        resultX = currentX;
        resultY = currentY;
        if (swapXY) {
            std::swap(resultX, resultY);
        }

        if (flipY) {
            resultY *= -1;
        }

        // 产生新顶点
        currentPoint.x = resultX;
        currentPoint.y = resultY;

        interpolantLine(start, end, currentPoint);

        results.push_back(currentPoint);
    }
}


void Raster::interpolantLine(const Point& v0, const Point& v1, Point& target) {
    // 计算插值权重： 
    // weight = 0：目标点在起点位置
    // weight = 1：目标点在终点位置
    // weight = 0.5：目标点在直线中点
    float weight = 1.0f;
    if (v1.x != v0.x) {
        // 用x做比例
        weight = (float)(target.x - v0.x) / (float)(v1.x - v0.x);
    } else if (v1.y != v0.y) {
        // 如果x方向上没有位移
        // 用y做比例
        weight = (float)(target.y - v0.y) / (float)(v1.y - v0.y);
    }

    RGBA result;
    // 目标颜色 = 终点颜色 × weight + 起点颜色 × (1 - weight)
    result.mR = static_cast<byte>(static_cast<float>(v1.color.mR) * weight + (1.0f - weight) * static_cast<float>(v0.color.mR));
    result.mG = static_cast<byte>(static_cast<float>(v1.color.mG) * weight + (1.0f - weight) * static_cast<float>(v0.color.mG));
    result.mB = static_cast<byte>(static_cast<float>(v1.color.mB) * weight + (1.0f - weight) * static_cast<float>(v0.color.mB));
    result.mA = static_cast<byte>(static_cast<float>(v1.color.mA) * weight + (1.0f - weight) * static_cast<float>(v0.color.mA));

    target.color = result;
}
