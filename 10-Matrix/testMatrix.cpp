#include "math/math.h"

int main() {
	math::Mat3f m33;
	auto m = m33 * 4.0f;
	m.printMatrix();

	auto n = m33;

	auto s = m * n;

	s.printMatrix();

	//math::Mat4f m41;
	//math::Mat4f m42;

	//auto m43 = m41 * m42;
	//m43.printMatrix();

	return 0;
}