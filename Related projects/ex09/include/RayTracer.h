#ifndef _RAYTRACER_H__
#define _RAYTRACER_H__

#include <memory>
#include <string>
#include <vector>

#include <boost/thread/thread.hpp>

#include "FrameBuffer.hpp"
#include "SceneObject.hpp"
#include "RayTracerState.hpp"
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
	  * Renders the current scene
	  */
	void render();

	/**
	  * Saves the currently rendered frame as an image file
	  */
	void save(std::string basename, std::string extension);

	void renderFrameArea(Screen screen_area, 
						std::shared_ptr<FrameBuffer> fb,
						std::shared_ptr<RayTracerState> state);
private:
	std::shared_ptr<FrameBuffer> fb;
	std::shared_ptr<RayTracerState> state;

	Screen screen;
};

#endif