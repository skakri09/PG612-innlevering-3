#ifndef SCENEOBJECTEFFECT_HPP__
#define SCENEOBJECTEFFECT_HPP__

#include "Ray.hpp"
#include "RayTracerState.hpp"
#include "Light.hpp"

/**
  * Abstract class that defines what it means to be an effect for a scene object
  */
class SceneObjectEffect {
public:
	/**
	  * This function "shades" an intersection point between a scene object
	  * and a ray. It can also fire new rays etc.
	  */
	virtual glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) = 0;
private:
};

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

/**
  * The phong effect simply uses phong shading to color the intersection point
  */
class PhongEffect : public SceneObjectEffect {
public:
	PhongEffect(glm::vec3 pos=glm::vec3(0.0),
				glm::vec3 diff=glm::vec3(0.5),
				glm::vec3 spec=glm::vec3(0.5)) {
		this->pos = pos;
		this->diff = diff;
		this->spec = spec;
	}

	glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) {
		glm::vec3 p = ray.getOrigin() + t*ray.getDirection();
		
		glm::vec3 v = glm::normalize(ray.getOrigin() - p);
		//glm::vec3 vv = glm::normalize(ray.getDirection());
		glm::vec3 l = glm::normalize(this->pos - p);
		/*glm::vec3 h = glm::normalize(glm::normalize(ray.getDirection()) + this->pos);*/
		glm::vec3 h = glm::normalize(v + l);
		//glm::vec3 h = glm::normalize(vv + this->pos);
		glm::vec3 n = glm::normalize(normal);

		float diffuse = glm::max(0.0f, glm::dot(n, l));
		float specular = glm::pow( glm::max(0.0f, glm::dot(n, h)), 30.0f);

		return glm::vec3( (diff*diffuse)+(spec*specular) );
	}


private:
	glm::vec3 pos;
	glm::vec3 diff;
	glm::vec3 spec;
};

/**
  * The shaded phong effect uses phong shading to color the intersection point, and applies shadows
  * if the object is in shadow
  */
class ShadedPhongEffect : public SceneObjectEffect {
public:

	glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) {
		std::vector<std::shared_ptr<LightObject>>::iterator iter;
		glm::vec3 color;

		glm::vec3 p = ray.getOrigin() + t*ray.getDirection();

		for(iter = state.getLights().begin(); iter != state.getLights().end(); iter++){
			float shadow_value;
			shadow_value = (*iter)->PointInShadow(p, state);
			//skipping contribution from this light if the object is fully shadowed by it
			if(shadow_value <=0.00001f)
				continue;

			glm::vec3 pos = (*iter)->position;

			glm::vec3 diff = (*iter)->diff;
			glm::vec3 spec = (*iter)->spec;

			glm::vec3 v = glm::normalize(ray.getOrigin() - p);
			glm::vec3 l = glm::normalize(pos - p);
			glm::vec3 h = glm::normalize(v+l);
			glm::vec3 n = glm::normalize(normal);

			float diffuse = glm::max(0.0f, glm::dot(n, l));
			float specular = glm::pow( glm::max(0.0f, glm::dot(n, h)), 50.0f);

			glm::vec3 new_color = glm::vec3( (diff*diffuse)+(spec*specular) ) * shadow_value ;
			color += new_color;
		}
		color/=state.getLights().size();
		//glm::vec3 absnormal = glm::vec3(abs(normal.x),abs(normal.y),abs(normal.z)); 
		return color;//*shadefactor;
	}


private:

};

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

class ReflectDarkEdgesEffect : public SceneObjectEffect{
public:
	ReflectDarkEdgesEffect(glm::vec3 pos=glm::vec3(0.0),
		glm::vec3 diff=glm::vec3(0.5),
		glm::vec3 spec=glm::vec3(0.5)) {
			this->pos = pos;
			this->diff = diff;
			this->spec = spec;
			absorb_amount = 0.05f;
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

enum SchlickMaterial{AIR, CARBONDIOXIDE, WATER, ETHANOL, PYREX, DIAMOND};

class FresnelEffect : public SceneObjectEffect{
public:
	FresnelEffect(SchlickMaterial environement, SchlickMaterial object)
	{
		eta_environment = get_eta(environement);
		eta_object = get_eta(object);
		eta_in = eta_environment/eta_object;
		eta_out = eta_object/eta_environment;
	}

	FresnelEffect(float environement, float object)
	{
		eta_environment = environement;
		eta_object = object;
		eta_in = eta_environment/eta_object;
		eta_out = eta_object/eta_environment;
	}

	glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) {

		float dot_product = glm::dot(normal, ray.getDirection());
		if(dot_product < 0.0f){
			//float R0 = glm::pow( (eta_in-eta_out) / (eta_in+eta_out), 2.0f);
			float R0 = glm::pow( (eta_object-eta_environment) / (eta_object+eta_environment), 2.0f);

			glm::vec3 n = glm::normalize(normal);
			glm::vec3 v = glm::normalize(ray.getDirection());

			//glm::vec3 refl_dir(glm::reflect(v, n));
			glm::vec3 refl_dir = reflect(n, v);

			//glm::vec3 refract_dir(glm::refract(v, n, eta_in));
			glm::vec3 refract_dir = refract(n, v, eta_in);

			float fresnel = R0 + (1.0f-R0)*glm::pow((1.0f-glm::dot(-v, n)), 5.0f);
			float reflect_contribution = ray.getColorContribution()*fresnel;
			float refract_contribution = ray.getColorContribution()*(1.0f-fresnel);

			glm::vec3 reflect = state.rayTrace(ray.spawn(t, refl_dir, reflect_contribution));
			//return reflect;
			glm::vec3 refract = state.rayTrace(ray.spawn(t, refract_dir, refract_contribution));
			//return refract;
			glm::vec3 out_color = glm::mix(refract, reflect, fresnel);
			return out_color;
		}
		else {
			return state.rayTrace(ray.spawn(t, ray.getDirection(), ray.getColorContribution()));
			float R0 = glm::pow( (eta_environment-eta_object) / (eta_environment+eta_object), 2.0f);

			glm::vec3 n = normal;//glm::normalize(normal);
			glm::vec3 v = ray.getDirection();//glm::normalize(ray.getDirection());

			glm::vec3 refl_dir(glm::reflect(v, -n));
			//glm::vec3 refract_dir(glm::refract(v, -n, eta_out));
			glm::vec3 refract_dir = refract(-n, v, eta_out);

			float fresnel = R0 + (1.0f-R0)*glm::pow((1.0f-glm::dot(-v, n)), 5.0f);
			float reflect_contribution = ray.getColorContribution()*fresnel;
			float refract_contribution = ray.getColorContribution()*(1.0f-fresnel);

			glm::vec3 reflect = state.rayTrace(ray.spawn(t, refl_dir, reflect_contribution));
			glm::vec3 refract = state.rayTrace(ray.spawn(t, refract_dir, refract_contribution));
			//return reflect;
			//return refract;
			return glm::mix(refract, reflect, fresnel);
		}
	}


private:
	static const float eta_air;
	static const float eta_carbondioxide;
	static const float eta_water;
	static const float eta_ethanol;
	static const float eta_pyrex;
	static const float eta_diamond;

	static float get_eta(SchlickMaterial material){
		switch(material){
		case AIR: return eta_air; break;
		case WATER: return eta_water; break;
		case PYREX: return eta_pyrex; break;
		case ETHANOL: return eta_ethanol; break;
		case DIAMOND: return eta_diamond; break;
		case CARBONDIOXIDE: return eta_carbondioxide; break;
		default: return eta_air;
		}
	}
	
	static inline glm::vec3 refract(glm::vec3& normal, glm::vec3& ray_dir, float eta){
		float w = eta*(glm::dot(-ray_dir, normal));
		float k = sqrt(1.0f+(w-eta)*(w+eta));
		return glm::normalize(glm::vec3( (w-k)*normal - (eta*(-ray_dir)) ) );
	}

	static inline glm::vec3 reflect(glm::vec3& normal, glm::vec3& ray_dir){
		return glm::vec3( 2 * glm::dot( normal, -ray_dir) * normal - (-ray_dir));
	}

	float eta_environment;
	float eta_object;
	float eta_in, eta_out;
};

const float FresnelEffect::eta_air = 1.000293f;
const float FresnelEffect::eta_carbondioxide = 1.00045f;
const float FresnelEffect::eta_water = 1.3330f;
const float FresnelEffect::eta_ethanol = 1.361f;
const float FresnelEffect::eta_pyrex = 1.470f;
const float FresnelEffect::eta_diamond = 2.419f;

#endif