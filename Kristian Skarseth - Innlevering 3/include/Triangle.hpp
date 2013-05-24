#ifndef _TRIANGLE_HPP
#define _TRIANGLE_HPP

#include "RayTracerState.hpp"
#include "SceneObject.hpp"
#include "SceneObjectEffect.hpp"

#include <glm/glm.hpp>

/**
  * The plane is a scene object 
  *
  * References:
  *				Real-Time Rendering (3rd ed) "Ray/Triangle intersection" p746-750
  *				Dan Sunday. 2012. http://geomalgorithms.com/a06-_intersect-2.html
  */
class Triangle : public SceneObject {
public:
	/**
	* Creates a triangle from the 3 corners p1, p2 and p3. For the normal of the surface to be correct,
	  the corners should be in counter clockwise order (as in openGL).
	*/
	Triangle(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, std::shared_ptr<SceneObjectEffect> effect)
		: p0(p0), p1(p1), p2(p2), SceneObject(effect)
	{
		glm::vec3 a, b;
		a = glm::normalize(p0-p1);
		b = glm::normalize(p2-p1);
		this->normal = glm::normalize(glm::cross(b, a));
		u = p1 - p0;
		v = p2 - p0;
	}

	float intersect(const Ray& r) {
		glm::vec3 dir, w0;
		float a, b;
		
		w0 = r.getOrigin() - p0; //vector from ray origin to one of the corners of the triangle
		a = -glm::dot(normal, w0);
		b = glm::dot(normal, r.getDirection());

		if (fabs(b) < 0.000001f) { // The ray is parallel to the triangle plane
			if (a == 0.0f){       
				return a;	   // the ray lies in triangle plane
			}
			else {
				return -1.0f;     // the ray is parallel to the plane, but not in it, thus it can never hit it.
			}
		}

		float q = a / b;
		if (q < 0.0f){
			return -1.0f; // The ray goes away from the triangle
		}
		glm::vec3 intersection_point = r.getOrigin() + q * r.getDirection(); // intersect point of ray and plane
		glm::vec3 w = intersection_point - p0;
		
		float uu = glm::dot(u, u);
		float uv = glm::dot(u, v);
		float vv = glm::dot(v, v);
		float wu = glm::dot(w, u);
		float wv = glm::dot(w, v);
		float d = uv * uv - uu * vv;

		// Testing if the intersection point is in the triangle
		float s = (uv * wv - vv * wu) / d;
		if (s < 0.0f || s > 1.0f){        
			return -1.0f; 
		}
		float t = (uv * wu - uu * wv) / d;
		if (t < 0.0f || (s + t) > 1.0f){
			return -1.0f;
		}

		//The intersection point is in the triangle
		return q	;
	}	

	glm::vec3 rayTrace(Ray &ray, const float& t, RayTracerState& state) {
		return effect->rayTrace(ray, t, normal, state);
	}

protected:
	glm::vec3 p0, p1, p2;
	glm::vec3 u, v; //Edges in the triangle
	glm::vec3 normal;
};

#endif