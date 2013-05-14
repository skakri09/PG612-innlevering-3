/********************************************************************
    created:    12:5:2013   14:29
    filename:   ReflectEffectDarkEdges.h
    author:     Kristian Skarseth
    
    purpose:    
*********************************************************************/
#ifndef ReflectEffectDarkEdges_h__
#define ReflectEffectDarkEdges_h__

#include "SceneObjectEffect.hpp"

class ReflectDarkEdgesEffect : public SceneObjectEffect{
public:
	ReflectDarkEdgesEffect(glm::vec3 pos=glm::vec3(0.0),
		glm::vec3 diff=glm::vec3(0.5),
		glm::vec3 spec=glm::vec3(0.5))
		: pos(pos), diff(diff), spec(spec),
		  absorb_amount(0.05f){
	}

	glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) {
		float dotval = glm::dot(normal, -ray.getDirection());
		float s = 0.7f;
		dotval = s*1.0f + (1.0f-s) * dotval;

		return state.rayTrace(ray.spawn(t, glm::reflect(ray.getDirection(), normal), ray.getColorContribution() - absorb_amount)) * dotval;
	}


private:
	glm::vec3 pos;
	glm::vec3 diff;
	glm::vec3 spec;
	float absorb_amount;
};

#endif // ReflectEffectDarkEdges_h__