/********************************************************************
    created:    12:5:2013   14:28
    filename:   ShadedPhongEffect.h
    author:     Kristian Skarseth
    
    purpose:    
*********************************************************************/
#ifndef ShadedPhongEffect_h__
#define ShadedPhongEffect_h__

//#include "SceneObjectEffect.hpp"

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
		return color;
	}


private:

};



#endif // ShadedPhongEffect_h__