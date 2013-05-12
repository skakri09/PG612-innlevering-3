/********************************************************************
    created:    12:5:2013   14:30
    filename:   ReflectEffect.h
    author:     Kristian Skarseth
    
    purpose:    
*********************************************************************/
#ifndef ReflectEffect_h__
#define ReflectEffect_h__

#include "SceneObjectEffect.hpp"

class ReflectEffect : public SceneObjectEffect{
public:
	ReflectEffect(glm::vec3 pos=glm::vec3(0.0),
		glm::vec3 diff=glm::vec3(0.5),
		glm::vec3 spec=glm::vec3(0.5)) {
			this->pos = pos;
			this->diff = diff;
			this->spec = spec;
			absorb_amount = 0.05f;
	}

	glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) {
		return state.rayTrace(ray.spawn(t, glm::reflect(ray.getDirection(), normal), ray.getColorContribution() - absorb_amount));
	}


private:
	glm::vec3 pos;
	glm::vec3 diff;
	glm::vec3 spec;
	float absorb_amount;
};

#endif // ReflectEffect_h__