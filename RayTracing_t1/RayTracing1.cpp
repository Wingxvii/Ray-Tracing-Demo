// RayTracing1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <float.h>

//simple image processing library CImg
#include "CImg.h"
#include "vec3.h"
#include "utils.h"
#include "ray.h"
#include "camera.h"

using namespace std;
using namespace cimg_library;


vec3 color(const ray &r, hittable *world, int depth) {
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec)) {
		ray scattered;
		vec3 attenuation;


		/*this is where we have definately hit an object with our ray*/
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * color(scattered, world, depth + 1);
		}
		else {
			return vec3(0, 0, 0);
		}

		//return 0.5f * vec3(rec.normal.x() + 1, rec.normal.y() + 1, rec.normal.z() + 1);
	}
	else {

		vec3 unit_direction = unit_vector(r.direction());
		//positive shift then halve to lock between 0 and 1
		float t = 0.5f * (unit_direction.y() + 1.0f);

		//linerally interpolate between white and our original color
		return (1.0f - t) * vec3(1.0f, 1.0f, 1.0f) + t * vec3(0.5f, 0.7f, 1.0f);
	}
}

class hittable_list : public hittable {
public:
	hittable_list() {}
	hittable_list(hittable** l, int n) {
		list = l;
		list_size = n;
	}

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
		hit_record temp_rec;
		bool hit_anything = false;
		double closest_so_far = t_max;

		for (int i = 0; i < list_size; i++) {
			if (list[i]->hit(r, t_min, closest_so_far, temp_rec)) {
				hit_anything = true;
				closest_so_far = temp_rec.t;
				rec = temp_rec;
			}
		}
		return hit_anything;
	}

	hittable** list;
	int list_size;

};

int main()
{
	int nx, ny, countRows;
	nx = 500;
	ny = 250;

	//create new image 1 plane, 3 colors
	CImg<unsigned char> img(nx, ny, 1, 3);
	//canvas for displaying image
	CImgDisplay canvas(img, "RayTracing Test2", 0);

	countRows = 0;
	//vec3 origin(0.0f, 0.0f, 0.0f);
	//vec3 lower_left_corner(-2.0, -1.0f,-1.0f);
	//vec3 horizontal(4.0f, 0.0f, 0.0f);
	//vec3 vertical(0.0f, 2.0f, 0.0f);

	hittable* list[2];
	list[0] = new sphere(vec3(0, 0, -1), 0.5f, new lambertian(vec3(0.8,0.3,0.3)));
	list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
	hittable* world = new hittable_list(list, 2); 

	camera cam;
	int numSamples = 100;

	//go through every pixel
	for (int j = ny - 1; j >= 0; j--, countRows++) {
		for (int i = 0; i < nx; i++) {
			vec3 col(0, 0, 0);

			for (int sample = 0; sample < numSamples; sample++) {
				float u = (i + random_double()) / float(nx);
				float v = float((ny - j) + random_double()) / float(ny);
				ray r = cam.get_ray(u, v);
				col += color(r, world, 0);
			}
			col /= float(numSamples);
			//float u = (float)(i) / (float)nx;
			//float v = (float)(j) / (float)ny;
			//shoot a ray from origin to every pixel
			//ray r(origin, lower_left_corner + u * horizontal + v * vertical);
			//compute color
			//vec3 col = color(r, world);

			//convert into 256 range
			int ir = int(255.99f * col[0]);
			int ig = int(255.99f * col[1]);
			int ib = int(255.99f * col[2]);
			
			//set channels
			img(i, j, 0, 0) = ir;
			img(i, j, 0, 1) = ig;
			img(i, j, 0, 2) = ib;
		}
		//progress output
		float percentDone = 100.f * (float(countRows) / float(ny - 1));
		cout << percentDone << "%" << endl;
	}
	canvas.resize(nx, ny);
	//flip to invert coordinate system to match windows
	//img.mirror('y');
	//display image
	while (!canvas.is_closed() && !canvas.is_keyESC()) {
		img.display(canvas);
		cimg::wait(20);	//20 ms per frame
	}
}

