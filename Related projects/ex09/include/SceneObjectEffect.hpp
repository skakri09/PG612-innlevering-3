#ifndef SCENEOBJECTEFFECT_HPP__
#define SCENEOBJECTEFFECT_HPP__

#include "Ray.hpp"
#include "RayTracerState.hpp"

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
		glm::vec3 l = glm::normalize(this->pos - p);
		glm::vec3 h = glm::normalize(glm::normalize(ray.getDirection()) + this->pos);
		glm::vec3 n = normal;

		float diffuse = glm::max(0.0f, glm::dot(n, l));
		float specular = glm::pow( glm::max(0.0f, glm::dot(n, h)), 50.0f);

		return glm::vec3( (diff*diffuse)+(spec*specular) );
		//return glm::vec3(abs(n.x), abs(n.y), abs(n.z));
	}


private:
	glm::vec3 pos;
	glm::vec3 diff;
	glm::vec3 spec;
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