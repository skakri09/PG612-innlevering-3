#include "RayTracer.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <sys/stat.h>

#include <IL/il.h>
#include <IL/ilu.h>

#include "CubeMap.hpp"

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
		glm::vec3 out_color(0.0, 0.0, 0.0);
		float t = std::numeric_limits<float>::max();
		float x, y, z;

		// Create the ray using the view screen definition 
		x = i*(screen.right-screen.left)/static_cast<float>(fb->getWidth()) + screen.left;
		y = j*(screen.top-screen.bottom)/static_cast<float>(fb->getHeight()) + screen.bottom;
		z = -1.0f;
		glm::vec3 direction = glm::vec3(x, y, z);
		Ray r = Ray(state->getCamPos(), direction);

		//Now do the ray-tracing to shade the pixel
		out_color = state->rayTrace(r);
		fb->setPixel(i, j, out_color);

		if(f >= thread_info->next_progress_target)
		{
			std::cout<< "Thread " << thread_info->thread_id << ": " 
				<< thread_info->thread_progress*100.0f << "%" << std::endl;
			thread_info->thread_progress+=0.1f;
			thread_info->next_progress_target = lerp(start_index, end_index, thread_info->thread_progress);
		}
	}
}

float RayTracer::lerp( int i0, int i1, float t )
{
	float v0 = (float)i0;
	float v1 = (float)i1;

	return v0*(1.0f-t)+v1*t;
}
