#ifndef _RAY_HPP__
#define _RAY_HPP__

#include <glm/glm.hpp>

/**
  * The ray class holds the state information of a ray in our ray-tracer:
  * point of origin and direction
  */
class Ray {
public:
	Ray(glm::vec3 origin, glm::vec3 direction, float color_contribution = 1.0f)
		: origin(origin), direction(direction), color_contribution(color_contribution), depth(0) {
	}

	/**
	  * Returns the origin of the ray
	  */
	inline const glm::vec3& getOrigin() const { return origin; }

	/**
	  * Returns the direction of the ray
	  */
	inline const glm::vec3& getDirection() const { return direction; }

	/**
	* Returns the final color contribution value of this ray
	*/
	inline float getColorContribution(){return color_contribution;}

	/**
	  * Spanws a new ray from this ray originating from getOrigin() + t*getDirection() 
	  * going in the direction of d
	  */
	inline Ray spawn(float t, glm::vec3 d, float color_contribution) const {
		Ray r(getOrigin()+(t+0.00001f)*getDirection(), d, color_contribution);
		r.depth = this->depth + 1;
		return r;
	}

	/**
	  * Tests whether or not this ray should be raytraced further
	  */
	inline bool isValid() const {
		return (color_contribution > 0.002f);
		//return (depth < max_depth);
	}


	/**
	  * Invalidate ray, saying it should not be raytraced further
	  */
	inline void invalidate() {
		this->depth = max_depth;
	}

private:
	friend class RayTracer;
	float color_contribution;

	static const unsigned int max_depth = 7;
	unsigned int depth;
	glm::vec3 origin;
	glm::vec3 direction;
};

#endif