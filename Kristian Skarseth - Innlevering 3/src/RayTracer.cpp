#include "RayTracer.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <sys/stat.h>
#include <omp.h>

#include <IL/il.h>
#include <IL/ilu.h>

#include "CubeMap.hpp"

/**
* References: http://www.codermind.com/articles/Raytracer-in-C++-Part-III-Textures.html
*/
RayTracer::RayTracer(unsigned int width, unsigned int height) {
	const glm::vec3 camera_position(0.0f, 0.0f, 10.0f);

	//Initialize framebuffer and virtual screen
	fb.reset(new FrameBuffer(width, height));
	float aspect = width/static_cast<float>(height);
	screen.top = 1.0f;
	screen.bottom = -1.0f;
	screen.right = aspect;
	screen.left = -aspect;

	//Initialize state
	state.reset(new RayTracerState(camera_position));

	//Initialize IL and ILU
	ilInit();
	iluInit();
}

void RayTracer::addSceneObject(std::shared_ptr<SceneObject>& o) {
	state->getScene().push_back(o);
}

void RayTracer::addLightSource(std::shared_ptr<LightObject>& light){
	state->getLights().push_back(light);
}

void RayTracer::render() {
	//For every pixel, ray-trace using multiple CPUs
	unsigned int rendered_pixels = 0;
	float progress = 0.0f;
	const unsigned int total_pixels = fb->getWidth()*fb->getHeight();
	float next_target = lerp(0, total_pixels, 0.01f);

#ifdef _OPENMP
#pragma omp parallel for
	for (int j=0; j<static_cast<int>(fb->getHeight()); ++j) {
#else
	for (unsigned int j=0; j<fb->getHeight(); ++j) {
#endif
		for (unsigned int i=0; i<fb->getWidth(); ++i) {
#pragma omp atomic
			rendered_pixels++;
			if(omp_get_thread_num() == 0){
				
				if(rendered_pixels >= next_target){
					progress+=1.0f;
					next_target = lerp(0, total_pixels, (progress+1.0f)/100.0f);
					std::cout << progress << "%" << std::endl;
				}
			}
				fb->setPixel(i,j, raytrace_16x_multisampled(i, j));
		}
	}
	std::cout << "100%" << std::endl;
}

void RayTracer::save(std::string basename, std::string extension) {
	ILuint texid;
	struct stat buffer;
	int i;
	std::stringstream filename;

	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);

	//Create image
	ilGenImages(1, &texid);
	ilBindImage(texid);
	//FIXME: Ugly const cast:( DevILs fault, unfortunately
	ilTexImage(fb->getWidth(), fb->getHeight(), 1, 3, IL_RGB, IL_FLOAT, const_cast<float*>(fb->getData().data()));

	//Find a unique filename...
	for (i=0; i<10000; ++i) {
		filename.str("");
		filename << basename << std::setw(4) << std::setfill('0') << i << "." << extension;
		if (stat(filename.str().c_str(), &buffer) != 0) break;
	}

	if (i == 10000) {
		std::stringstream log;
		log << "Unable to find unique filename for " << basename << "%d." << extension;
		throw std::runtime_error(log.str());
	}

	if (!ilSaveImage(filename.str().c_str())) {
		std::stringstream log;
		log << "Unable to save " << filename.str();
		throw std::runtime_error(log.str());
	}
	else {
		std::cout << "Saved " << filename.str() << std::endl;
	}

	ilDeleteImages(1, &texid);
}

float RayTracer::lerp( int i0, int i1, float t )
{
	float v0 = (float)i0;
	float v1 = (float)i1;

	return v0*(1.0f-t)+v1*t;
}

const glm::vec2 RayTracer::sample_16x_values[] = {
	glm::vec2(-0.50, 0.50), glm::vec2(-0.25, 0.50),glm::vec2(0.125, 0.50),glm::vec2(0.50, 0.50),
	glm::vec2(-0.50, 0.25), glm::vec2(-0.25, 0.25),glm::vec2(0.125, 0.125),glm::vec2(0.50, 0.125),

	glm::vec2(-0.50, -0.25), glm::vec2(-0.25, -0.25),glm::vec2(0.25, -0.25),glm::vec2(0.50, -0.25),
	glm::vec2(-0.50, -0.50), glm::vec2(-0.25, -0.50),glm::vec2(0.25, -0.50),glm::vec2(0.50, -0.50)
};
const std::size_t RayTracer::sample_16x_array_length = sizeof(sample_16x_values)/sizeof(sample_16x_values[0]);

const glm::vec2 RayTracer::sample_64x_values[] = {
	glm::vec2(-0.50, 0.50), glm::vec2(-0.375, 0.50),glm::vec2(-0.25, 0.50),glm::vec2(-0.125, 0.50),glm::vec2(-0.125, 0.50), glm::vec2(-0.25, 0.50),glm::vec2(0.375, 0.50),glm::vec2(0.50, 0.50),
	glm::vec2(-0.50, 0.375), glm::vec2(-0.375, 0.375),glm::vec2(-0.25, 0.375),glm::vec2(-0.125, 0.375),glm::vec2(-0.125, 0.375), glm::vec2(-0.25, 0.375),glm::vec2(0.375, 0.375),glm::vec2(0.50, 0.375),
	glm::vec2(-0.50, 0.25), glm::vec2(-0.375, 0.25),glm::vec2(-0.25, 0.25),glm::vec2(-0.125, 0.25),glm::vec2(-0.125, 0.25), glm::vec2(-0.25, 0.25),glm::vec2(0.375, 0.25),glm::vec2(0.50, 0.25),
	glm::vec2(-0.50, 0.125), glm::vec2(-0.375, 0.125),glm::vec2(-0.25, 0.125),glm::vec2(-0.125, 0.125),glm::vec2(-0.125, 0.125), glm::vec2(-0.25, 0.125),glm::vec2(0.375, 0.125),glm::vec2(0.50, 0.125),

	glm::vec2(-0.50, -0.125), glm::vec2(-0.375, -0.125),glm::vec2(-0.25, -0.125),glm::vec2(-0.125, -0.125),glm::vec2(-0.125, -0.125), glm::vec2(-0.25, -0.125),glm::vec2(0.375, -0.125),glm::vec2(0.50, -0.125),
	glm::vec2(-0.50, -0.25), glm::vec2(-0.375, -0.25),glm::vec2(-0.25, -0.25),glm::vec2(-0.125, -0.25),glm::vec2(-0.125, -0.25), glm::vec2(-0.25, -0.25),glm::vec2(0.375, -0.25),glm::vec2(0.50, -0.25),
	glm::vec2(-0.50, -0.375), glm::vec2(-0.375, -0.375),glm::vec2(-0.25, -0.375),glm::vec2(-0.125, -0.375),glm::vec2(-0.125, -0.375), glm::vec2(-0.25, -0.375),glm::vec2(0.375, -0.375),glm::vec2(0.50-0.375),
	glm::vec2(-0.50, -0.50), glm::vec2(-0.375, -0.50),glm::vec2(-0.25, -0.50),glm::vec2(-0.125, -0.50),glm::vec2(-0.125, -0.50), glm::vec2(-0.25, -0.50),glm::vec2(0.375, -0.50),glm::vec2(0.50, -0.50)
};
const std::size_t RayTracer::sample_64x_array_length = sizeof(sample_64x_values)/sizeof(sample_64x_values[0]);

glm::vec3 RayTracer::raytrace_4x_multisampled( unsigned int i, unsigned int j )
{
	float x, y, z;
	z = -1.0f;

	x = (i-0.25f)*(screen.right-screen.left)/static_cast<float>(fb->getWidth()) + screen.left;
	y = (j-0.25f)*(screen.top-screen.bottom)/static_cast<float>(fb->getHeight()) + screen.bottom;
	glm::vec3 d1 = glm::vec3(x, y, z);
	Ray r1 = Ray(state->getCamPos(), d1);

	x = (i-0.25f)*(screen.right-screen.left)/static_cast<float>(fb->getWidth()) + screen.left;
	y = (j+0.25f)*(screen.top-screen.bottom)/static_cast<float>(fb->getHeight()) + screen.bottom;
	glm::vec3 d2 = glm::vec3(x, y, z);
	Ray r2 = Ray(state->getCamPos(), d2);

	x = (i+0.25f)*(screen.right-screen.left)/static_cast<float>(fb->getWidth()) + screen.left;
	y = (j+0.25f)*(screen.top-screen.bottom)/static_cast<float>(fb->getHeight()) + screen.bottom;
	glm::vec3 d3 = glm::vec3(x, y, z);
	Ray r3 = Ray(state->getCamPos(), d3);

	x = (i+0.25f)*(screen.right-screen.left)/static_cast<float>(fb->getWidth()) + screen.left;
	y = (j-0.25f)*(screen.top-screen.bottom)/static_cast<float>(fb->getHeight()) + screen.bottom;

	glm::vec3 d4 = glm::vec3(x, y, z);
	Ray r4 = Ray(state->getCamPos(), d4);

	return 0.25f*(state->rayTrace(r1)+state->rayTrace(r2)+state->rayTrace(r3)+state->rayTrace(r4)) ;
}

glm::vec3 RayTracer::raytrace_1x_sampled( unsigned int i, unsigned int j )
{
	float x, y, z;
	z = -1.0f;

	x = i*(screen.right-screen.left)/static_cast<float>(fb->getWidth()) + screen.left;
	y = j*(screen.top-screen.bottom)/static_cast<float>(fb->getHeight()) + screen.bottom;
	glm::vec3 d1 = glm::vec3(x, y, z);
	Ray r1 = Ray(state->getCamPos(), d1);

	return state->rayTrace(r1);
}

glm::vec3 RayTracer::raytrace_16x_multisampled( unsigned int i, unsigned int j )
{
	glm::vec3 dir(0, 0, -1.0f);
	glm::vec3 color(0.0f);

	for(std::size_t t = 0; t < sample_16x_array_length; t++){
		dir.x = (i+sample_16x_values[t].x)*(screen.right-screen.left)/static_cast<float>(fb->getWidth()) + screen.left;
		dir.y = (j+sample_16x_values[t].y)*(screen.top-screen.bottom)/static_cast<float>(fb->getHeight()) + screen.bottom;
		Ray ray = Ray(state->getCamPos(), dir);
		color+=state->rayTrace(ray);
	}
	//Now do the ray-tracing to shade the pixel
	return (1.0f/sample_16x_array_length)*color;
}

glm::vec3 RayTracer::raytrace_64x_multisampled( unsigned int i, unsigned int j )
{
	glm::vec3 dir(0, 0, -1.0f);
	glm::vec3 color(0.0f);

	for(std::size_t t = 0; t < sample_64x_array_length; t++){
		dir.x = (i+sample_64x_values[t].x)*(screen.right-screen.left)/static_cast<float>(fb->getWidth()) + screen.left;
		dir.y = (j+sample_64x_values[t].y)*(screen.top-screen.bottom)/static_cast<float>(fb->getHeight()) + screen.bottom;
		Ray ray = Ray(state->getCamPos(), dir);
		color+=state->rayTrace(ray);
	}
	return (1.0f/sample_64x_array_length)*color;
}
