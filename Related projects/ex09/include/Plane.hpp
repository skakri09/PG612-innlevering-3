#ifndef _PLANE_HPP_
#define _PLANE_HPP_

#include "RayTracerState.hpp"
#include "SceneObject.hpp"
#include "SceneObjectEffect.hpp"

#include <glm/glm.hpp>

/**
  * The plane is a scene object 
  */
class Plane : public SceneObject {
public:
	Plane(glm::vec3 center, float radius, std::shared_ptr<SceneObjectEffect> effect) {
		this->p = center;
		this->r = radius;
		this->effect = effect;
	}

	/**
	  * Computes the ray-sphere intersection
	  */
	float intersect(const Ray& r) {
		const float z_offset = 10e-4f;
		const glm::vec3 d = r.getDirection();
		const glm::vec3 p0 = r.getOrigin();
		float a = glm::dot(d, d);
		float b = 2.0f*glm::dot(d, (p0-p));
		float c = glm::dot(p0-p, p0-p)-this->r*this->r;
		

		//Just test whether or not we hit the ray to display something at least
		float baseval = b*b-4.0f*a*c;

		if (baseval >= 0.0f) {
			float sqrt_val = sqrt(baseval);
			float t_1 = ( -b + sqrt_val ) / (2*a);
			float t_2 = ( -b - sqrt_val ) / (2*a);

			if(t_1*t_2 < 0.0f)
				return glm::max(t_1,t_2);
			else if(t_1 > 0.0f && t_2 > 0.0f)
				return glm::min(t_1, t_2);
			else 
				return -1.0f;
		}
		else{//No intersections
			return -1;
		}
	}	
	
	/**
	  * Computes normal for an intersection point on a sphere
	  */
	const glm::vec3 computeNormal(const Ray& r, const float& t) {
		return glm::vec3( ( (r.getOrigin() + t*r.getDirection()) - this->p) / this->r);
	}

	glm::vec3 rayTrace(Ray &ray, const float& t, RayTracerState& state) {
		glm::vec3 normal = computeNormal(ray, t);
		glm::vec3 p = ray.getOrigin() + (t*ray.getDirection());

		return effect->rayTrace(ray, t, normal, state);
	}

protected:
	glm::vec3 p; //< center of sphere
	float r;   //< sphere radius
};

#endif