#ifndef _TRIANGLE_HPP
#define _TRIANGLE_HPP

#include "RayTracerState.hpp"
#include "SceneObject.hpp"
#include "SceneObjectEffect.hpp"

#include <glm/glm.hpp>

/**
  * The plane is a scene object 
  *
  * References: http://geomalgorithms.com/a06-_intersect-2.html
  *				http://www.blackpawn.com/texts/pointinpoly/default.html
  */
class Triangle : public SceneObject {
public:
	/**
	* Creates a triangle from the 3 corners p1, p2 and p3. For the normal of the surface to be correct,
	  the corners should be in counter clockwise order (as in openGL).
	*/
	Triangle(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, std::shared_ptr<SceneObjectEffect> effect) {
		this->p0 = p0;
		this->p1 = p1;
		this->p2 = p2;
		glm::vec3 a, b;
		a = glm::normalize(p0-p1);
		b = glm::normalize(p2-p1);
		this->normal = glm::cross(a, b);
	}

	/**
	  * Computes the ray-sphere intersection
	  *
	  * @returns 1.0f if there is intersection, 0.0f if there is no intersection
	  */
	float intersect(const Ray& r) {
		//const float z_offset = 10e-4f;
		const glm::vec3 d = r.getDirection();
		const glm::vec3 ray_start = r.getOrigin();

		float dot00, dot01, dot02, dot11, dot12;
		glm::vec3 v0, v1, v2;

		float t = glm::dot((p1 - ray_start), (normal / (glm::dot(d,normal))));

		glm::vec3 q = p0 + (t*d); //triangle plane intersection point

		v0 = p2-p0;
		v1 = p1-p0;
		v2 = q-p0;
		
		dot00 = glm::dot(v0, v0);
		dot01 = glm::dot(v0, v1);
		dot02 = glm::dot(v0, v2);
		dot11 = glm::dot(v1, v1);
		dot12 = glm::dot(v1, v2);
		
		float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
		float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		if( (u >= 0) && (v >= 0) && (u + v < 1) ){
			return 1.0f;
		}
		return 0.0f;
	}	

	glm::vec3 rayTrace(Ray &ray, const float& t, RayTracerState& state) {
		
		glm::vec3 p = ray.getOrigin() + (t*ray.getDirection());

		return effect->rayTrace(ray, t, normal, state);
	}

protected:
	glm::vec3 p0, p1, p2;
	glm::vec3 normal;
};

#endif