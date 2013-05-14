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
  *				Real-Time Rendering (3rd ed) "Ray/Triangle intersection" p746-750. 
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
		this->normal = glm::cross(b, a);
		normal = glm::normalize(normal);
	}

	float intersect(const Ray& r) {
		glm::vec3 u, v, n;
		glm::vec3 dir, w0, w;
		float q, a, b;
		glm::vec3 intersection_point;
		
		u = p1 - p0; //Edge in the triangle
		v = p2 - p0; //Edge in the triangle
		n = normal;  //Triangle normal
		
		dir = r.getDirection(); // ray direction vector
		w0 = r.getOrigin() - p0;//vector from ray origin to one of the corners of the triangle
		a = -glm::dot(n,w0);
		b = glm::dot(n,dir);

		if (fabs(b) < 0.000001f) { // The ray is parallel to the triangle plane
			if (a == 0.0f)         
				return 0.0f;	   // the ray lies in triangle plane
			else return -1.0f;     // the ray is parallel to the plane, but not in it, thus it can never hit it.
		}

		q = a / b;
		if (q < 0.0f) 
			return -1; // The ray goes away from the triangle

		intersection_point = r.getOrigin() + q * dir; // intersect point of ray and plane

		
		float uu, uv, vv, wu, wv, D;
		uu = glm::dot(u,u);
		uv = glm::dot(u,v);
		vv = glm::dot(v,v);
		w = intersection_point - p0;
		wu = glm::dot(w,u);
		wv = glm::dot(w,v);
		D = uv * uv - uu * vv;

		// Testing if the intersection point is in the triangle
		float s = (uv * wv - vv * wu) / D;
		if (s < 0.0f || s > 1.0f)        
			return -1.0f; 
		float t = (uv * wu - uu * wv) / D;
		if (t < 0.0f || (s + t) > 1.0f)
			return -1.0f;

		//The intersection point is in the triangle
		return q;
	}	

	glm::vec3 rayTrace(Ray &ray, const float& t, RayTracerState& state) {
		return effect->rayTrace(ray, t, normal, state);
	}

protected:
	glm::vec3 p0, p1, p2;
	glm::vec3 normal;
};

#endif