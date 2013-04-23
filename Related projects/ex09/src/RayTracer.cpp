#include "RayTracer.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <sys/stat.h>

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
//#ifdef _OPENMP
//#pragma omp parallel for
//	for (int j=0; j<static_cast<int>(fb->getHeight()); ++j) {
//#else
//	for (unsigned int j=0; j<fb->getHeight(); ++j) {
//#endif

	std::vector<ScreenCoord> screen_coords;
	screen_coords.resize(fb->getHeight()*fb->getWidth());
	unsigned int counter = 0;
	for (unsigned int j=0; j<fb->getHeight(); ++j) {
		for (unsigned int i=0; i<fb->getWidth(); ++i) {
			screen_coords.at(counter).x = j;
			screen_coords.at(counter).y = i;
			counter++;
		}
	}

	unsigned int index_count = 0;
	std::vector<std::shared_ptr<Thread>> threads; 
	unsigned int threads_number = boost::thread::hardware_concurrency();
	unsigned int offset = floor( (float)screen_coords.size() / (float)threads_number );

	for(unsigned int i = 0; i < threads_number; ++i)
	{
		if(i == (threads_number-1) )
		{
			offset = (screen_coords.size()) - index_count;
		}
		std::shared_ptr<Thread> new_thread = std::make_shared<Thread>();
		new_thread->thread.reset(new boost::thread(std::bind(&RayTracer::renderFrameArea, this, 
												&screen_coords, index_count, index_count + offset, new_thread)));
		new_thread->thread_id = i;
		new_thread->thread_progress = 0.1f;
		new_thread->next_progress_target = lerp(index_count, index_count+offset, new_thread->thread_progress);
		threads.push_back(new_thread);
		index_count+=offset;
	}

	for(unsigned int i = 0; i < threads.size(); i++){
		threads.at(i)->thread->join();
	}
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

void RayTracer::renderFrameArea( std::vector<ScreenCoord>* screen_coords, 
	unsigned int start_index, unsigned int end_index,
	std::shared_ptr<Thread> thread_info)
{
	int workload = end_index-start_index;
	for(unsigned int f = start_index; f < end_index; f++)
	{
		unsigned int i = screen_coords->at(f).y;
		unsigned int j = screen_coords->at(f).x;
		//glm::vec3 out_color(0.0, 0.0, 0.0);
		//float t = std::numeric_limits<float>::max();
		

		fb->setPixel(i, j, raytrace_16x_multisampled(i, j) );

		if(f >= thread_info->next_progress_target)
		{
			std::cout<< "Thread " << thread_info->thread_id << ": " 
				<< thread_info->thread_progress*100.0f << "%" << std::endl;
			thread_info->thread_progress+=0.1f;
			thread_info->next_progress_target = lerp(start_index, end_index, thread_info->thread_progress);
		}
	}
	std::cout<< "Thread " << thread_info->thread_id << " done."<< std::endl;
}

float RayTracer::lerp( int i0, int i1, float t )
{
	float v0 = (float)i0;
	float v1 = (float)i1;

	return v0*(1.0f-t)+v1*t;
}

const glm::vec2 RayTracer::sample_16x_values[] = {
	glm::vec2(-0.25, 0.25), glm::vec2(-0.125, 0.25),glm::vec2(0.125, 0.25),glm::vec2(0.25, 0.25),
	glm::vec2(-0.25, 0.125), glm::vec2(-0.125, 0.125),glm::vec2(0.125, 0.125),glm::vec2(0.25, 0.125),

	glm::vec2(-0.25, -0.125), glm::vec2(-0.125, -0.125),glm::vec2(0.125, -0.125),glm::vec2(0.25, -0.125),
	glm::vec2(-0.25, -0.25), glm::vec2(-0.125, -0.25),glm::vec2(0.125, -0.25),glm::vec2(0.25, -0.25)
};
const std::size_t RayTracer::sample_16x_array_length = sizeof(sample_16x_values)/sizeof(sample_16x_values[0]);

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

	//Now do the ray-tracing to shade the pixel
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
	//x = (i-0.25f)*(screen.right-screen.left)/static_cast<float>(fb->getWidth()) + screen.left;
	//y = (j-0.25f)*(screen.top-screen.bottom)/static_cast<float>(fb->getHeight()) + screen.bottom;
	//glm::vec3 d1 = glm::vec3(x, y, z);
	//Ray r1 = Ray(state->getCamPos(), d1);

	//x = (i-0.25f)*(screen.right-screen.left)/static_cast<float>(fb->getWidth()) + screen.left;
	//y = (j+0.25f)*(screen.top-screen.bottom)/static_cast<float>(fb->getHeight()) + screen.bottom;
	//glm::vec3 d2 = glm::vec3(x, y, z);
	//Ray r2 = Ray(state->getCamPos(), d2);

	//x = (i+0.25f)*(screen.right-screen.left)/static_cast<float>(fb->getWidth()) + screen.left;
	//y = (j+0.25f)*(screen.top-screen.bottom)/static_cast<float>(fb->getHeight()) + screen.bottom;
	//glm::vec3 d3 = glm::vec3(x, y, z);
	//Ray r3 = Ray(state->getCamPos(), d3);

	//x = (i+0.25f)*(screen.right-screen.left)/static_cast<float>(fb->getWidth()) + screen.left;
	//y = (j-0.25f)*(screen.top-screen.bottom)/static_cast<float>(fb->getHeight()) + screen.bottom;
	//glm::vec3 d4 = glm::vec3(x, y, z);
	//Ray r4 = Ray(state->getCamPos(), d4);

	for(std::size_t t = 0; t < sample_16x_array_length; t++){
		dir.x = (i+sample_16x_values[t].x)*(screen.right-screen.left)/static_cast<float>(fb->getWidth()) + screen.left;
		dir.y = (j+sample_16x_values[t].y)*(screen.top-screen.bottom)/static_cast<float>(fb->getHeight()) + screen.bottom;
		Ray ray = Ray(state->getCamPos(), dir);
		color+=state->rayTrace(ray);
	}
	//Now do the ray-tracing to shade the pixel
	return (1.0f/sample_16x_array_length)*color;
}
