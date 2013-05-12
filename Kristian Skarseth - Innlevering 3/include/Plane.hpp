/********************************************************************
    created:    12:5:2013   14:31
    filename:   Plane.h
    author:     Kristian Skarseth
    
    purpose:    
*********************************************************************/
#ifndef Plane_h__
#define Plane_h__


class Plane
{
public:
/**
	* Creates a triangle from the 3 corners p1, p2 and p3. For the normal of the surface to be correct,
	  the corners should be in counter clockwise order (as in openGL).
	*/
	Plane(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, std::shared_ptr<SceneObjectEffect> effect) {
		this->p0 = p0;
		this->p1 = p1;
		this->p2 = p2;
		glm::vec3 a, b;
		a = glm::normalize(p0-p1);
		b = glm::normalize(p2-p1);
		this->normal = glm::cross(b, a);
		normal = glm::normalize(normal);
		this->effect = effect;
	}

	float intersect(const Ray& r) {
		
	}	

	glm::vec3 rayTrace(Ray &ray, const float& t, RayTracerState& state) {

		return effect->rayTrace(ray, t, normal, state);
	}

protected:
	glm::vec3 p0, p1, p2;
	glm::vec3 normal;
};

#endif // Plane_h__