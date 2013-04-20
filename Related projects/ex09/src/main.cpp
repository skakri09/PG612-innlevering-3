#include <iostream>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <stdlib.h> 

#include "RayTracer.h"
#include "Sphere.hpp"
#include "Light.hpp"
#include "CubeMap.hpp"
#include "Timer.h"


/**
 * Simple program that starts our game manager
 */
int main(int argc, char *argv[]) {
	try {
		RayTracer* rt;
		Timer t;
		rt = new RayTracer(1920, 1080);
		
		std::shared_ptr<SceneObjectEffect> color(new ColorEffect(glm::vec3(0.0, 1.0, 0.0)));
		std::shared_ptr<SceneObjectEffect> phong(new PhongEffect(glm::vec3(0.0, 0.0, 10.0)));
		std::shared_ptr<SceneObjectEffect> reflect(new ReflectEffect(glm::vec3(0.0, 0.0, 10.0)));
		std::shared_ptr<SceneObjectEffect> phongshaded(new ShadedPhongEffect());

		std::shared_ptr<LightObject> light1(new PointLight(glm::vec3(0.0f, 0.0f, 15.0f)));
		rt->addLightSource(light1);
		//std::shared_ptr<LightObject> light2(new PointLight(glm::vec3(-8.0f, 0.0f, 10.0f)));
		//rt->addLightSource(light2);
		//srand(static_cast<int>(time(NULL)));
		//for (int i=0; i<10; ++i) {
		//	float tx = (rand() / (float) RAND_MAX - 0.5f) *15;
		//	float ty = (rand() / (float) RAND_MAX - 0.5f) *15;
		//	float tz = (rand() / (float) RAND_MAX - 0.5f) *5;

		//	std::shared_ptr<SceneObject> s1(new Sphere(glm::vec3(-tx, ty, tz), 2.0f, phongshaded));
		//	rt->addSceneObject(s1);
		//}
		std::shared_ptr<SceneObject> s1(new Sphere(glm::vec3(-3.0f, 0.0f, 5.0f), 2.0f, phongshaded));
		rt->addSceneObject(s1);
		std::shared_ptr<SceneObject> s2(new Sphere(glm::vec3(3.0f, 0.0f, 2.0f), 2.0f, phongshaded));
		rt->addSceneObject(s2);
		std::shared_ptr<SceneObject> s3(new Sphere(glm::vec3(0.0f, 3.0f, 4.0f), 2.0f, phongshaded));
		rt->addSceneObject(s3);
				
		t.restart();
		rt->render();
		double elapsed = t.elapsed();
		std::cout << "Computed in " << elapsed << " seconds" <<  std::endl;
		rt->save("test", "bmp"); //We want to write out bmp's to get proper bit-maps (jpeg encoding is lossy)

		delete rt;
	} catch (std::exception &e) {
		std::string err = e.what();
		std::cout << err.c_str() << std::endl;
		return -1;
	}
	return 0;
}
