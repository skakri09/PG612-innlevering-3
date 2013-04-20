/********************************************************************
    created:    20:4:2013   13:25
    filename:   Light.h
    author:     Kristian Skarseth
    
    purpose:    
*********************************************************************/
#ifndef Light_h__
#define Light_h__

#include "RayTracerState.hpp"
#include "SceneObject.hpp"
#include "SceneObjectEffect.hpp"

#include <glm/glm.hpp>

class LightObject
{
public:
	
	/**
	* Performs a test to see if the parameter point is in line of sight(los)
	* of this light.
	*
	* @param point the point of we perform the los check from
	* @returns -1 if the light is in line of sight of the point (no shadow).
	*          If the light is not a point light, the returned value varies between
			   [0, 1] to indicate the softness of the shadow created at the point.
	*/
	virtual float PointInShadow(const glm::vec3& point, RayTracerState& state) = 0;

};

/**
* The PointLight is a scene object that defines a point light
*/
class PointLight : public LightObject
{
public:
    PointLight(glm::vec3 position){
		this->position = position;
	}

	
	float PointInShadow(const glm::vec3& point, RayTracerState& state){
		Ray ray(point, position-point);
		glm::vec3 ray_result = state.rayTrace(ray);
		if(ray_result == glm::vec3(0.3f))
			return -1.0f;//no shadow
		else return 1.0f;//full shadow
	}

private:
	glm::vec3 position;
};

#endif // Light_h__