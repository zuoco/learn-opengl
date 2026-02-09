#pragma once
#include "../global/base.h"
#include "../math/math.h"

struct Point {
	int32_t x{ 0 };
	int32_t y{ 0 };
	RGBA color;
	math::vec2f uv;
};