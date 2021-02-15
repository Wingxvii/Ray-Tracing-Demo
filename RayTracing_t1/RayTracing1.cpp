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

int nx, ny, countRows;
hittable* world;
int totalSamples;
const int MAX_SAMPLES = 100;
float movespeed = 0.2;
float turnrate = 0.5;

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

void update(CImg<unsigned char>& img, camera cam) {
	for (int j = ny - 1; j >= 0; j--, countRows++) {
		for (int i = 0; i < nx; i++) {
			vec3 col(0, 0, 0);

			//recall, then unsaturate prior pixel color based on equal ratio if image already exists
			if (totalSamples != 0) {
				col = vec3(img(i, j, 0, 0), img(i, j, 0, 1), img(i, j, 0, 2));
				col *= (totalSamples - 1) / (float)totalSamples;
			}

			float u = (i + random_double()) / float(nx);
			float v = float((ny - j) + random_double()) / float(ny);
			ray r = cam.get_ray(u, v);

			//add new color, ratio with existing samples, and convert into 256 range
			col[0] += int(255.99f * color(r, world, 0)[0] / (float)totalSamples);
			col[1] += int(255.99f * color(r, world, 0)[1] / (float)totalSamples);
			col[2] += int(255.99f * color(r, world, 0)[2] / (float)totalSamples);

			//set channels
			img(i, j, 0, 0) = col[0];
			img(i, j, 0, 1) = col[1];
			img(i, j, 0, 2) = col[2];
		}
		////progress output
		//float percentDone = 100.f * (float(countRows) / float(ny - 1));
	}

	totalSamples++;
	cout << totalSamples << " Samples" << endl;
}

int main()
{
	nx = 1000;
	ny = 500;
	totalSamples = 0;

	//create new image 1 plane, 3 colors
	CImg<unsigned char> img(nx, ny, 1, 3);
	//canvas for displaying image
	CImgDisplay canvas(img, "RayTracing Test2", 0);

	countRows = 0;
	hittable* list[4];
	list[0] = new sphere(vec3(0, 0, -1), 0.5f, new lambertian(vec3(0.8,0.3,0.3)));
	list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
	list[2] = new sphere(vec3(1, 0, -1), 0.5f, new metal(vec3(0.8, 0.8, 0.8)));
	list[3] = new sphere(vec3(0.5, 0.5, -1), 0.25f, new metal(vec3(0.8, 0.8, 0.8)));

	world = new hittable_list(list, 4); 

	vec3 lookfrom(3, 3, 2);
	vec3 lookat(0, 0, -1);
	float dist_to_focus = (lookfrom - lookat).length();
	float aperature = 0.01f;

	camera cam(lookfrom, lookat, vec3(0,1,0), 20, float(nx)/float(ny), aperature, dist_to_focus);

	//go through every pixel

	canvas.resize(nx, ny);

	//flip to invert coordinate system to match windows
	//img.mirror('y');
	//display image
	while (!canvas.is_closed() && !canvas.is_keyESC()) {

		/*Movement check*/
		if (canvas.is_keyW()) {
			cam.move(vec3(0, movespeed, 0));
			totalSamples = 0;
		}
		else if (canvas.is_keyS()) {
			cam.move(vec3(0, -movespeed, 0));
			totalSamples = 0;
		}
		else if (canvas.is_keyA()) {
			cam.move(vec3(0, 0, movespeed));
			totalSamples = 0;
		}
		else if (canvas.is_keyD()) {
			cam.move(vec3(0, 0, -movespeed));
			totalSamples = 0;
		}
		else if (canvas.is_keyQ()) {
			cam.move(vec3(movespeed, 0, 0));
			totalSamples = 0;
		}
		else if (canvas.is_keyE()) {
			cam.move(vec3(-movespeed, 0, 0));
			totalSamples = 0;
		}
		if (canvas.is_keyI()) {
			cam.move(vec3(0, turnrate, 0));
			totalSamples = 0;
		}
		else if (canvas.is_keyK()) {
			cam.turn(vec3(0, -turnrate, 0));
			totalSamples = 0;
		}
		else if (canvas.is_keyJ()) {
			cam.turn(vec3(0, 0, turnrate));
			totalSamples = 0;
		}
		else if (canvas.is_keyL()) {
			cam.turn(vec3(0, 0, -turnrate));
			totalSamples = 0;
		}
		else if (canvas.is_keyU()) {
			cam.turn(vec3(turnrate, 0, 0));
			totalSamples = 0;
		}
		else if (canvas.is_keyO()) {
			cam.turn(vec3(-turnrate, 0, 0));
			totalSamples = 0;
		}

		if (totalSamples < MAX_SAMPLES) {
			update(img, cam);
		}

		img.display(canvas);
	}
}

//challenges:

//create an intresting scene with 100 spheres, use loop*****
//plays with different shapes (cube)
//play with different refractions, or sub-surface scattereing (ggx)
//increase resolution
//play with samples per pixel
//move camera around/ objects around procedurally and render each frame to make a video*****
//volumetric effects like smoke