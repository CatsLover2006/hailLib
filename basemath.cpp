#include "basemath.hpp"

#include <cmath>
#include <cstdint>

namespace hailMath {
	double pi = acos(-1);

	float abs_q (float x) {
		int32_t y = *((int32_t*)&x);
		y = y & 0x7fffffff; // Remove the signature bit
		x = *((float*)&y);
		return x;
	}

	double abs_q (double x) {
		int64_t y = *((int64_t*)&x);
		y = y & 0x7fffffffffffffff; // Remove the signature bit
		x = *((double*)&y);
		return x;
	}
}