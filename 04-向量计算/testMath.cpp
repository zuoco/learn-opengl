#include "global/base.h"
#include "math/math.h"

using namespace math;
int main() {
	vec2f v2_1(2.0f,3.0f);
	vec2f v2_2 = v2_1;

	auto v2_add = v2_1 + v2_2;
	std::cout << "v2 add is:" << std::endl;
	v2_add.print();

	auto v2_multi = v2_1 * v2_2;
	std::cout << "v2 multi is:" << std::endl;
	v2_multi.print();


	auto ss = 2.0 * v2_1;
	ss.print();

	auto v2_div = v2_2 / 2;
	std::cout << "v2 div is:" << std::endl;
	v2_div.print();

	vec4f v4(5.0f, 1.0f, 3.0f, 1.0f);
	auto v4n = normalize(v4);
	v4n.print();

	return 0;
}