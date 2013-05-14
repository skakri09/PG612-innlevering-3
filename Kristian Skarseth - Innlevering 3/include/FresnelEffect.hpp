/********************************************************************
    created:    12:5:2013   14:29
    filename:   FresnelEffect.h
    author:     Kristian Skarseth
    
    purpose:    Giving objects a fresnel effect by defining the environment and object
				refractive indexes.
*********************************************************************/
#ifndef FresnelEffect_h__
#define FresnelEffect_h__

#include "SceneObjectEffect.hpp"
#include "RefractionIndices.h"


class FresnelEffect : public SceneObjectEffect{
public:
	/**
	* @param environment the refractive index of the environment (default to air)
	* @param object the refractive index of the objects using this effect
	*/
	FresnelEffect(float object, float environement = Snell::Air)
		:eta_environment(environement), eta_object(object)
	{
		eta_environment = environement;
		eta_object = object;
		eta_in = eta_environment/eta_object;
		eta_out = eta_object/eta_environment;

		RF0_in = glm::pow( (eta_object-eta_environment) / (eta_object+eta_environment), 2.0f);
		RF0_out = glm::pow( (eta_environment-eta_object) / (eta_environment+eta_object), 2.0f);
	}

	/**
	* The fresnel rayTrace function creates a fresnel effect for the objects it affects.
	*/
	glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) {

		glm::vec3 n = glm::normalize(normal);
		glm::vec3 v = glm::normalize(ray.getDirection());

		if(glm::dot(n, v) < 0.0f){
			glm::vec3 refl_dir = reflect(n, v);
			glm::vec3 refract_dir = refract(n, v, eta_in);

			float fresnel = RF0_in + (1.0f-RF0_in)*glm::pow((1.0f-glm::dot(-v, n)), 5.0f);

			float reflect_contribution = ray.getColorContribution()*fresnel;
			float refract_contribution = ray.getColorContribution()*(1.0f-fresnel);

			glm::vec3 reflect = state.rayTrace(ray.spawn(t, refl_dir, reflect_contribution));
			glm::vec3 refract = state.rayTrace(ray.spawn(t, refract_dir, refract_contribution));

			return glm::mix(refract, reflect, fresnel);
		}
		else {
			glm::vec3 refl_dir(glm::reflect(v, n));
			glm::vec3 refract_dir = refract(-n, v, eta_out);

			float fresnel = RF0_out + (1.0f-RF0_out)*glm::pow((1.0f-glm::dot(refract_dir, n)), 5.0f);
			
			float reflect_contribution = ray.getColorContribution()*fresnel;
			float refract_contribution = ray.getColorContribution()*(1.0f-fresnel);

			glm::vec3 reflect = state.rayTrace(ray.spawn(t, refl_dir, reflect_contribution));
			glm::vec3 refract = state.rayTrace(ray.spawn(t, refract_dir, refract_contribution));

			return glm::mix(refract, reflect, fresnel);
		}
	}


private:
	/**
	* Refracts a vector
	*
	* @normal the normal of the surface/point we are refracting through
	* @ray_dir the original direction of the vector we wish to refract
	* @eta the refractive index computed by dividing the index of the 
		   material we are coming from, with the material we are entering
	* @return a vector refracted based on the function parameters
	*/
	static inline glm::vec3 refract(glm::vec3& normal, glm::vec3& dir_vector, float eta){
		float w = eta*(glm::dot(-dir_vector, normal));
		float k = sqrt(1.0f+(w-eta)*(w+eta));
		return glm::normalize(glm::vec3( (w-k)*normal - (eta*(-dir_vector)) ) );
	}

	/**
	* Reflects a vector
	*
	* @normal the normal of the surface/point we are reflecting on
	* @ray_vector the original direction of the vector we are reflecting
	* @return a vector reflected around the normal, based on the original ray_vector
	*/
	static inline glm::vec3 reflect(glm::vec3& normal, glm::vec3& ray_vector){
		return glm::vec3( 2.0f * glm::dot( normal, -ray_vector) * normal - (-ray_vector));
	}

	float eta_environment;
	float eta_object;
	float eta_in, eta_out;
	float RF0_in, RF0_out;
};

#endif // FresnelEffect_h__