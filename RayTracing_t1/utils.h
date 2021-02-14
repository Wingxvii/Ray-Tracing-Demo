#pragma once
#include "vec3.h"
#define M_PI 3.14159265358979323846264338327950288


#include <cstdlib>

inline double random_double() {
	return rand() / (RAND_MAX + 1.0);
}

vec3 random_in_unit_disk() {
	vec3 p;
	do {
		p = 2.0f * vec3(random_double(), random_double(), 0) - vec3(1, 1, 0);
	} while (dot(p, p) >= 1.0);
	return p;
}

