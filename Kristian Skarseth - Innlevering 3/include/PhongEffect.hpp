/********************************************************************
    created:    12:5:2013   14:28
    filename:   PhongEffect.h
    author:     Kristian Skarseth
    
    purpose:    
*********************************************************************/
#ifndef PhongEffect_h__
#define PhongEffect_h__

#include "SceneObjectEffect.hpp"

/**
  * The phong effect simply uses phong shading to color the intersection point
  */
class PhongEffect : public SceneObjectEffect {
public:
	PhongEffect(glm::vec3 pos=glm::vec3(0.0),
				glm::vec3 diff=glm::vec3(0.5),
				glm::vec3 spec=glm::vec3(0.5))
				pos(pos), diff(diff), spec(spec){
		this->pos = pos;
		this->diff = diff;
		this->spec = spec;
	}

	glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) {
		glm::vec3 p = ray.getOrigin() + t*ray.getDirection();
		
		glm::vec3 v = glm::normalize(ray.getOrigin() - p);
		glm::vec3 l = glm::normalize(this->pos - p);
		glm::vec3 h = glm::normalize(v + l);
		glm::vec3 n = glm::normalize(normal);

		float diffuse = glm::max(0.0f, glm::dot(n, l));
		float specular = glm::pow( glm::max(0.0f, glm::dot(n, h)), 30.0f);

		return glm::vec3( (diff*diffuse) + (spec*specular) );
	}


private:
	glm::vec3 pos;
	glm::vec3 diff;
	glm::vec3 spec;
};

#endif // PhongEffect_h__