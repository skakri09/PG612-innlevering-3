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
		this->normal = glm::cross(b, a);
		normal = glm::normalize(normal);
		this->effect = effect;
	}

	/**
	  * Computes the ray-sphere intersection
	  *
	  * @returns 1.0f if there is intersection, 0.0f if there is no intersection
	  */
	//float intersect(const Ray& r) {
	//	//const float z_offset = 10e-4f;
	//	const glm::vec3 d = r.getDirection();
	//	const glm::vec3 ray_start = r.getOrigin();

	//	float dot00, dot01, dot02, dot11, dot12;
	//	glm::vec3 v0, v1, v2;

	//	float t = glm::dot((p1 - ray_start), (normal / (glm::dot(d,normal))));

	//	glm::vec3 q = p0 + (t*d); //triangle plane intersection point

	//	v0 = p1-p0;
	//	v1 = p2-p0;
	//	v2 = q-p0;
	//	/*v0 = p2-p0;
	//	v1 = p1-p0;
	//	v2 = q-p0;*/
	//	
	//	dot00 = glm::dot(v0, v0);
	//	dot01 = glm::dot(v0, v1);
	//	dot02 = glm::dot(v0, v2);
	//	dot11 = glm::dot(v1, v1);
	//	dot12 = glm::dot(v1, v2);
	//	
	//	float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
	//	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	//	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	//	if( (u >= 0) && (v >= 0) && (u + v < 1) ){
	//		return 1.0f;
	//	}
	//	return 0.0f;	
	//}	
	float intersect(const Ray& r) {
		glm::vec3    u, v, n;              // triangle vectors
		glm::vec3    dir, w0, w;           // ray vectors
		float     _r, a, b;              // params to calc ray-plane intersect
		glm::vec3 intersection_point;
		// get triangle edge vectors and plane normal
		u = p1 - p0;
		v = p2 - p0;
		n = normal;
		
		dir = r.getDirection();//R.P1 - R.P0;              // ray direction vector
		w0 = r.getOrigin() - p0;//R.P0 - T.V0;
		a = -glm::dot(n,w0);
		b = glm::dot(n,dir);
		if (fabs(b) < 0.000001f) {     // ray is  parallel to triangle plane
			if (a == 0)                 // ray lies in triangle plane
				return 2;
			else return 0;              // ray disjoint from plane
		}

		// get intersect point of ray with triangle plane
		_r = a / b;
		if (_r < 0.0)                    // ray goes away from triangle
			return 0;                   // => no intersect
		// for a segment, also test if (r > 1.0) => no intersect

		intersection_point = r.getOrigin() + _r * dir;            // intersect point of ray and plane

		// is I inside T?
		float    uu, uv, vv, wu, wv, D;
		uu = glm::dot(u,u);
		uv = glm::dot(u,v);
		vv = glm::dot(v,v);
		w = intersection_point - p0;//T.V0;
		wu = glm::dot(w,u);
		wv = glm::dot(w,v);
		D = uv * uv - uu * vv;

		// get and test parametric coords
		float s, t;
		s = (uv * wv - vv * wu) / D;
		if (s < 0.0 || s > 1.0)         // I is outside T
			return 0;
		t = (uv * wu - uu * wv) / D;
		if (t < 0.0 || (s + t) > 1.0)  // I is outside T
			return 0;

		return 1;                       // I is in T
	}	

	glm::vec3 rayTrace(Ray &ray, const float& t, RayTracerState& state) {
		
		glm::vec3 p = ray.getOrigin() + (t*ray.getDirection());

		return normal;//effect->rayTrace(ray, t, normal, state);
	}

protected:
	glm::vec3 p0, p1, p2;
	glm::vec3 normal;
};

#endif