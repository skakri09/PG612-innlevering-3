#ifndef _TRIANGLE_HPP
#define _TRIANGLE_HPP

#include "RayTracerState.hpp"
#include "SceneObject.hpp"
#include "SceneObjectEffect.hpp"

#include <glm/glm.hpp>

/**
  * The plane is a scene object 
  */
class Triangle : public SceneObject {
public:
	/**
	* Creates a triangle from the 3 corners p1, p2 and p3. For the normal of the surface to be correct,
	  the corners should be in counter clockwise order (as in openGL).
	*/
	Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, std::shared_ptr<SceneObjectEffect> effect) {
		this->p1 = p1;
		this->p2 = p2;
		this->p3 = p3;
		
		glm::vec3 a = glm::normalize(p1-p2);
		glm::vec3 b = glm::normalize(p3-p2);
		this->normal = glm::cross(a, b);
	}

	/**
	  * Computes the ray-sphere intersection
	  */
	float intersect(const Ray& r) {
		//const float z_offset = 10e-4f;
		const glm::vec3 d = r.getDirection();
		const glm::vec3 p0 = r.getOrigin();

		float t = glm::dot((p1 - p0), (normal / (glm::dot(d,normal))));

		glm::vec3 q = p0 + (t*d); //triangle plane intersection point

	}	

	glm::vec3 rayTrace(Ray &ray, const float& t, RayTracerState& state) {
		;
		glm::vec3 p = ray.getOrigin() + (t*ray.getDirection());

		return effect->rayTrace(ray, t, normal, state);
	}

protected:
	glm::vec3 p1, p2, p3;
	glm::vec3 normal;
};

#endif