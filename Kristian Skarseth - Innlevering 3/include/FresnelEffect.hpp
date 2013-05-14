/********************************************************************
    created:    12:5:2013   14:29
    filename:   FresnelEffect.h
    author:     Kristian Skarseth
    
    purpose:    
*********************************************************************/
#ifndef FresnelEffect_h__
#define FresnelEffect_h__

#include "SceneObjectEffect.hpp"
#include "RefractionIndices.h"


class FresnelEffect : public SceneObjectEffect{
public:
	FresnelEffect(float environement, float object)
	{
		eta_environment = environement;
		eta_object = object;
		eta_in = eta_environment/eta_object;
		eta_out = eta_object/eta_environment;
	}

	glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) {

		glm::vec3 n = glm::normalize(normal);
		glm::vec3 v = glm::normalize(ray.getDirection());

		float dot_product = glm::dot(n, v);
		float my_t = t;
		if(dot_product < 0.0f){
			//float R0 = glm::pow( (eta_in-eta_out) / (eta_in+eta_out), 2.0f);
			float R0 = glm::pow( (eta_object-eta_environment) / (eta_object+eta_environment), 2.0f);

			//glm::vec3 refl_dir(glm::reflect(v, n));
			glm::vec3 refl_dir = reflect(n, v);

			//glm::vec3 refract_dir(glm::refract(v, n, eta_in));
			glm::vec3 refract_dir = refract(n, v, eta_in);

			float fresnel = R0 + (1.0f-R0)*glm::pow((1.0f-glm::dot(-v, n)), 5.0f);
			//fresnel = glm::clamp(fresnel, 0.0f, 1.0f);
			float reflect_contribution = ray.getColorContribution()*fresnel;
			float refract_contribution = ray.getColorContribution()*(1.0f-fresnel);

			glm::vec3 reflect = state.rayTrace(ray.spawn(my_t, refl_dir, reflect_contribution));
			//return ray.spawn(t, refract_dir, refract_contribution).getDirection();
			//return refract_dir;
			glm::vec3 refract = state.rayTrace(ray.spawn(my_t, refract_dir, refract_contribution));
			//return refract;
			glm::vec3 out_color = glm::mix(refract, reflect, fresnel);
			return out_color;
		}
		else {
			//return glm::vec3(0.5f);
			//return state.rayTrace(ray.spawn(t, ray.getDirection(), ray.getColorContribution()));
			float R0 = glm::pow( (eta_environment-eta_object) / (eta_environment+eta_object), 2.0f);
			//return glm::vec3(1.0f);
			//return normal;

			glm::vec3 refl_dir(glm::reflect(v, n));
			//glm::vec3 refract_dir(glm::refract(v, -n, eta_out));
			glm::vec3 refract_dir = refract(-n, v, eta_out);

			float fresnel = R0 + (1.0f-R0)*glm::pow((1.0f-glm::dot(refract_dir, n)), 5.0f);

			//float fresnel = R0 + (1.0f-R0)*glm::pow((1.0f-glm::dot(-v, n)), 5.0f);
			//fresnel = glm::clamp(fresnel, 0.0f, 1.0f);
			
			float reflect_contribution = ray.getColorContribution()*fresnel;
			float refract_contribution = ray.getColorContribution()*(1.0f-fresnel);

			glm::vec3 reflect = state.rayTrace(ray.spawn(my_t, refl_dir, reflect_contribution));
			glm::vec3 refract = state.rayTrace(ray.spawn(my_t, refract_dir, refract_contribution));
			//return reflect;
			//return refract;
			return glm::mix(refract, reflect, fresnel);
		}
	}


private:
	static inline glm::vec3 refract(glm::vec3& normal, glm::vec3& ray_dir, float eta){
		float w = eta*(glm::dot(-ray_dir, normal));
		float k = sqrt(1.0f+(w-eta)*(w+eta));
		return glm::normalize(glm::vec3( (w-k)*normal - (eta*(-ray_dir)) ) );
	}

	static inline glm::vec3 reflect(glm::vec3& normal, glm::vec3& ray_dir){
		return glm::vec3( 2 * glm::dot( normal, -ray_dir) * normal - (-ray_dir));
	}

	static inline glm::vec3 test_refract(glm::vec3 normal, glm::vec3 ray_dir, float eta_1, float eta_2){
		const float n = eta_1/eta_2;
		const float cosI = glm::dot(normal, ray_dir);
		const float sinT2 = n * n * (1.0f - cosI * cosI);
		if(sinT2 > 1.0f){
			return glm::vec3();
		}
		else{
			return n * ray_dir - (n + sqrt(1.0f - sinT2)) * normal;
		}
	}

	float eta_environment;
	float eta_object;
	float eta_in, eta_out;
};

#endif // FresnelEffect_h__