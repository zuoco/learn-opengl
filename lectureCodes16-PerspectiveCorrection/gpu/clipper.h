#pragma once
#include "dataStructures.h"

class Clipper {
public:
	static void doClipSpace(const uint32_t& drawMode, const std::vector<VsOutput>& primitives, std::vector<VsOutput>& outputs);

private:
	static void sutherlandHodgman(const uint32_t& drawMode, const std::vector<VsOutput>& primitive, std::vector<VsOutput>& outputs);

	static bool inside(const math::vec4f& point, const math::vec4f& plane);

	static VsOutput intersect(const VsOutput& last, const VsOutput& current, const math::vec4f& plane);
};