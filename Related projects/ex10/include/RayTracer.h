#ifndef _RAYTRACER_H__
#define _RAYTRACER_H__

#include <memory>
#include <string>
#include <vector>

#include "FrameBuffer.hpp"
#include "SceneObject.hpp"
#include "RayTracerState.hpp"
#include "Light.h"

#include <cmath>
#include <boost/thread/thread.hpp>

/**
* Defines the virtual screen we project our rays through
*/
struct Screen{
	Screen(float left, float right, float top, float bottom){
		this->left = left;
		this->right = right;
		this->top = top;
		this->bottom = bottom;
	}
	Screen(){}
	float left;
	float right;
	float top;
	float bottom;
};
struct ScreenCoord{
	ScreenCoord(unsigned int x, unsigned int y){
		this->x = x;
		this->y = y;
	}
	ScreenCoord(){}
	unsigned int x;
	unsigned int y;
};

struct Thread{
	std::shared_ptr<boost::thread> thread;
	int thread_id;
	float thread_progress;
	float next_progress_target;
};

/**
  * The RayTracer class is the main entry point for raytracing
  * our scene, saving results to file, etc.
  */
class RayTracer {
public:
	RayTracer(unsigned int width, unsigned int height);

	/**
	  * Adds an object to the scene
	  */
	void addSceneObject(std::shared_ptr<SceneObject>& o);

	/**
	* Adds a light to the scene
	*/
	void addLightSource(std::shared_ptr<LightObject>& light);

	/**
	  * Renders the current scene
	  */
	void render();

	/**
	  * Saves the currently rendered frame as an image file
	  */
	void save(std::string basename, std::string extension);

	void renderFrameArea(std::vector<ScreenCoord>* screen_coords, 
		unsigned int start_index, unsigned int end_index,
		std::shared_ptr<Thread> thread_info);

private:
	std::shared_ptr<FrameBuffer> fb;
	std::shared_ptr<RayTracerState> state;

	Screen screen;

	float lerp(int i0, int i1, float t);
};

#endif