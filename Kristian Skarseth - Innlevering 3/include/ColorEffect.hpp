/********************************************************************
    created:    12:5:2013   14:28
    filename:   ColorEffect.h
    author:     Kristian Skarseth
    
    purpose:    
*********************************************************************/
#ifndef ColorEffect_h__
#define ColorEffect_h__

#include "SceneObjectEffect.hpp"

/**
  * The color effect simply colors any point with the same color
  */
class ColorEffect : public SceneObjectEffect {
public:
	ColorEffect(glm::vec3 color) {
		this->color = color;
	}

	glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) {
		return color;
	}

private:
	glm::vec3 color;
};

#endif // ColorEffect_h__