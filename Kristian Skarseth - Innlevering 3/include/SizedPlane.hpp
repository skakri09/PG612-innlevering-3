/********************************************************************
    created:    12:5:2013   14:31
    filename:   Plane.h
    author:     Kristian Skarseth
    
    purpose:    
*********************************************************************/
#ifndef Plane_h__
#define Plane_h__

#include "RayTracerState.hpp"
#include "SceneObject.hpp"
#include "SceneObjectEffect.hpp"

#include <glm/glm.hpp>

class SizedPlane : public SceneObject
{
public:
/**
	* Creates a plane from the 4 corners p0, p1, p2 and p3. For the normal of the surface to be correct,
	  the corners should be in counter clockwise order (as in openGL).
	*/
	SizedPlane(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, std::shared_ptr<SceneObjectEffect> effect) {
		this->p0 = p0;
		this->p1 = p1;
		this->p2 = p2;
		this->p3 = p3;
		glm::vec3 a, b;
		a = glm::normalize(p0-p1);
		b = glm::normalize(p2-p1);
		this->normal = glm::cross(b, a);
		normal = glm::normalize(normal);
		this->effect = effect;

		float absX = abs(normal.x);
		float absY = abs(normal.y);
		float absZ = abs(normal.z);
		if(absX > absY){
			if(absX > absZ){
				ip0 = glm::vec2(p0.y, p0.z);
				ip1 = glm::vec2(p1.y, p1.z);
				ip2 = glm::vec2(p2.y, p2.z);
				ip3 = glm::vec2(p3.y, p3.z);
				throwaway_index= 0;
			}
			else{
				ip0 = glm::vec2(p0.x, p0.y);
				ip1 = glm::vec2(p1.x, p1.y);
				ip2 = glm::vec2(p2.x, p2.y);
				ip3 = glm::vec2(p3.x, p3.y);
				throwaway_index= 2;
			}
		}
		else{
			if(absY > absZ){
				ip0 = glm::vec2(p0.x, p0.z);
				ip1 = glm::vec2(p1.x, p1.z);
				ip2 = glm::vec2(p2.x, p2.z);
				ip3 = glm::vec2(p3.x, p3.z);
				throwaway_index= 1;
			}
			else{
				ip0 = glm::vec2(p0.x, p0.y);
				ip1 = glm::vec2(p1.x, p1.y);
				ip2 = glm::vec2(p2.x, p2.y);
				ip3 = glm::vec2(p3.x, p3.y);
				throwaway_index= 2;
			}
		}

		minp = glm::vec2(glm::min(ip0.x, glm::min(ip1.x, glm::min(ip2.x, ip3.x))),
						glm::min(ip0.y, glm::min(ip1.y, glm::min(ip2.y, ip3.y))));
		maxp = glm::vec2(glm::max(ip0.x, glm::max(ip1.x, glm::max(ip2.x, ip3.x))),
						glm::max(ip0.y, glm::max(ip1.y, glm::max(ip2.y, ip3.y))));
	}

	float intersect(const Ray& r) {
		glm::vec3 w0;
		float q, a, b;
		glm::vec3 intersection_point;

		w0 = r.getOrigin() - p0; //vector from ray origin to a point on the plane
		a = -glm::dot(normal,w0);
		b = glm::dot(normal,r.getDirection());

		if (fabs(b) < 0.000001f) { // The ray is parallel to the triangle plane
			if (a == 0.0f)         
				return 0.0f;	   // the ray lies in triangle plane
			else return -1.0f;     // the ray is parallel to the plane, but not in it, thus it can never hit it.
		}

		q = a / b;
		if (q < 0.0f) 
			return -1; // The ray goes away from the triangle

		intersection_point = r.getOrigin() + q * r.getDirection(); // intersect point of ray and plane
		glm::vec2 ip_2D;

		if(throwaway_index == 0)
			ip_2D = glm::vec2(intersection_point.y, intersection_point.z);
		else if(throwaway_index == 1)
			ip_2D = glm::vec2(intersection_point.x, intersection_point.z);
		else
			ip_2D = glm::vec2(intersection_point.x, intersection_point.y);

		if((ip_2D.x < maxp.x) && (ip_2D.y < maxp.y) && (ip_2D.x > minp.x) && (ip_2D.y > minp.y)){
			return q;
		}
		return -1.0f;
	}	

	glm::vec3 rayTrace(Ray &ray, const float& t, RayTracerState& state) {

		return effect->rayTrace(ray, t, normal, state);
	}

protected:
	glm::vec3 p0, p1, p2, p3;
	glm::vec2 ip0, ip1, ip2, ip3;
	glm::vec2 minp, maxp;
	glm::vec3 normal;
	int throwaway_index;
};

#endif // Plane_h__