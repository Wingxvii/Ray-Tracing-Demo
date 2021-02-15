#pragma once
#include "vec3.h"
#include "ray.h"
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

struct hit_record {
	float t;
	vec3 p;
	vec3 normal;

};

class hittable {
public:
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
};

class sphere : public hittable {
public:
	sphere() {}
	sphere(vec3 cen, float r) : center(cen), radius(r) {};

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
		{
			/* ray collision with a sphere - returns true if ray intersects with the sphere, false otherwise */
			vec3 oc = r.origin() - center;
			float a = dot(r.direction(), r.direction());
			float b = 2.0f * dot(r.direction(), oc);
			float c = dot(oc, oc) - radius * radius;
			float discrimiant = b * b - 4.0f * a * c;

			if (discrimiant < 0) {
				return false;
			}
			else {
				/* checking for the first solution b^2 -4ac*/
				float temp = ((-b - sqrt(discrimiant)) / (2.0f * a));
				if (temp < t_max && temp > t_min) {
					rec.t = temp;
					rec.p = r.point_at_parameter(rec.t);
					rec.normal = (rec.p - center) / radius;
					return true;
				}

				/*check for the second solution: b^2 + 4ac*/
				temp = ((-b + sqrt(discrimiant)) / (2.0f * a));
				if (temp < t_max && temp > t_min) {
					rec.t = temp;
					rec.p = r.point_at_parameter(rec.t);
					rec.normal = (rec.p - center) / radius;
					return true;
				}

				return false;
			}
		}
	}

	/* members */
	vec3 center;
	float radius;
};