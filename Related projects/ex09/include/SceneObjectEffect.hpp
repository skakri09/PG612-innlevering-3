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
			color += new_color;//glm::mix(color, new_color, 0.5f) ;


			//for(unsigned int i = 0; i < state.getLights().size(); i++){
			//	
			//	float s = state.getLights().at(i)->PointInShadow(p, state);
			//	if(s >= 0.0f)
			//		shadefactor+=s;
			//}
			//if(shadefactor > 0.0f){
			//	new_color = glm::vec3( (diff*diffuse) );
			//	shadefactor = 0.3f;//shadefactor * 0.25f + 0.75f;
			//}
			//else {
			//	
			//	shadefactor = 1.0f;
			//}
			//color*=shadefactor;
		}
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
	}

	glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) {
		glm::vec3 p = ray.getOrigin() + (t*ray.getDirection());
		Ray reflected_ray(p, glm::reflect(ray.getDirection(), normal));

		float dotval = glm::dot(normal, -ray.getDirection());
		float s = 0.7f;
		dotval = s*1.0f + (1.0f-s) * dotval;

		return state.rayTrace(reflected_ray) * dotval;
	}


private:
	glm::vec3 pos;
	glm::vec3 diff;
	glm::vec3 spec;

};

#endif