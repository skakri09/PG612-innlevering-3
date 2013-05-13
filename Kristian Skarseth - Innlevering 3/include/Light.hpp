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
	LightObject(glm::vec3 pos=glm::vec3(0.0),
				glm::vec3 diff=glm::vec3(0.5),
				glm::vec3 spec=glm::vec3(0.5)){
		this->position = pos;
		this->diff = diff;
		this->spec = spec;
	}
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

	glm::vec3 position;
	glm::vec3 diff;
	glm::vec3 spec;
};
 
/**
* The PointLight is a scene object that defines a point light
*/
class PointLight : public LightObject
{
public:
	PointLight(glm::vec3 pos=glm::vec3(0.0),
		glm::vec3 diff=glm::vec3(0.5),
		glm::vec3 spec=glm::vec3(0.5))
		:LightObject(pos, diff, spec){
	}

	
	float PointInShadow(const glm::vec3& point, RayTracerState& state){
		Ray shadow_ray(point, position-point);

		const float z_offset = 10e-4f;
		float t = -1;
		float t_min = std::numeric_limits<float>::max();
		int k_min=-1;
		//Loop through all the objects, to find the closest intersection, if any
		for (unsigned int k=0; k<state.getScene().size(); ++k) {
			t = state.getScene().at(k)->intersect(shadow_ray);
			
			//skipping the cubemap
			if(t >= (std::numeric_limits<float>::max()))
				continue;
			
			if (t > z_offset && t <= t_min) {
				k_min = k;
				t_min = t;
			}
		}

		if (k_min >= 0) {
			glm::vec3 q = point + shadow_ray.getDirection()*t_min;
			float light_length = glm::length(position-point);
			float q_lenght = glm::length(q);

			if(q_lenght < light_length)
				return 0.0f;
		}
		return 1.0f;
	}

private:

};


/**
* The PointLight is a scene object that defines a point light
*/
class SphereLight : public LightObject
{
public:
	SphereLight(glm::vec3 pos=glm::vec3(0.0),
		float radius = 1.0f,
		glm::vec3 diff=glm::vec3(0.5),
		glm::vec3 spec=glm::vec3(0.5))
		:LightObject(pos, diff, spec),
		radius_(radius){
	}


	float PointInShadow(const glm::vec3& point, RayTracerState& state){
		Ray shadow_ray(point, position-point);

		const float z_offset = 10e-4f;
		float t = -1;
		float t_min = std::numeric_limits<float>::max();
		int k_min=-1;
		//Loop through all the objects, to find the closest intersection, if any
		for (unsigned int k=0; k<state.getScene().size(); ++k) {
			t = state.getScene().at(k)->intersect(shadow_ray);

			//skipping the cubemap
			if(t >= (std::numeric_limits<float>::max()))
				continue;

			if (t > z_offset && t <= t_min) {
				k_min = k;
				t_min = t;
			}
		}

		if (k_min >= 0) {
			glm::vec3 q = point + shadow_ray.getDirection()*t_min;
			float light_length = glm::length(position-point);
			float q_lenght = glm::length(q);

			if(q_lenght < light_length)
				return 0.0f;
		}
		return 1.0f;
	}

private:
	float radius_;
};

#endif // Light_h__