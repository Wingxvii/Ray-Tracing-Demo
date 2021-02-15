#pragma once
#include "vec3.h"
#include "ray.h"
//==================================================================================================
// Written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is distributed
// without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication along
// with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==================================================================================================


class camera {
public:
	camera() {
		lower_left_corner = vec3(-2.0, -1.0, -1.0);
		horizontal = vec3(4.0, 0.0, 0.0);
		vertical = vec3(0.0, 2.0, 0.0);
		origin = vec3(0.0, 0.0, 0.0);
	}

	camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist) {
		lens_radius = aperture / 2.0f;
		float theta = vfov * M_PI / 180.0f;
		float half_height = tan(theta / 2);
		float half_width = aspect * half_height;
		origin = lookfrom;
		//this is our camera coordinate system
		//direction vecctor
		w = unit_vector(lookfrom - lookat);
		//left/right vector
		u = unit_vector(cross(vup, w));
		//new up vector
		v = cross(w, u);
		//fector operations based on coordinate system and focal length to give us image corner
		lower_left_corner = origin - half_width * focus_dist * u - half_height * focus_dist * v - focus_dist * w;
		//image directions in x/y
		horizontal = 2 * half_width * focus_dist * u;
		vertical = 2 * half_height * focus_dist * v;
	}


	ray get_ray(float s, float t) {
		vec3 rd = lens_radius * random_in_unit_disk();
		vec3 offset = u * rd.x() + v * rd.y();
		return ray(origin + offset, lower_left_corner + s*horizontal + t*vertical - origin - offset);

		//return ray(origin,
		//	lower_left_corner + u * horizontal + v * vertical - origin);
	}

	vec3 origin;
	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;

	vec3 u, v, w;
	float lens_radius;

};