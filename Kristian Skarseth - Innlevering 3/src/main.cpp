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
#include "Triangle.hpp"

/**
 * Simple program that starts our game manager
 */
int main(int argc, char *argv[]) {
	try {
		RayTracer* rt;
		Timer t;
		unsigned int width = 1920;
		unsigned int height = 1080;
		rt = new RayTracer(width, height);
		float aspect = width/static_cast<float>(height);

		//std::shared_ptr<SceneObjectEffect> color(new ColorEffect(glm::vec3(0.0, 1.0, 0.0)));
		std::shared_ptr<SceneObjectEffect> phong(new PhongEffect(glm::vec3(-5.0, 0.0, 9.0)));
		std::shared_ptr<SceneObjectEffect> reflect(new ReflectEffect(glm::vec3(0.0, 0.0, 10.0)));
		std::shared_ptr<SceneObjectEffect> phongshaded(new ShadedPhongEffect());
		std::shared_ptr<SceneObjectEffect> fresnel(new FresnelEffect(AIR, PYREX));
	
		std::shared_ptr<LightObject> light1(new PointLight(glm::vec3(-5.0f, 0.0f, 9.0f)));
		rt->addLightSource(light1);
		std::shared_ptr<LightObject> light2(new PointLight(glm::vec3(1.0f, 5.0f, 9.0f)));
		rt->addLightSource(light2);
		std::shared_ptr<LightObject> light3(new PointLight(glm::vec3(-1.0f, -5.0f, 9.0f)));
		rt->addLightSource(light3);
		
	/*	srand(static_cast<int>(time(NULL)));
		for (int i=0; i<10; ++i) {
			float tx = (rand() / (float) RAND_MAX - 0.5f) *25;
			float ty = (rand() / (float) RAND_MAX - 0.5f) *25;
			float tz = (rand() / (float) RAND_MAX - 0.5f) *15;

			std::shared_ptr<SceneObject> s1(new Sphere(glm::vec3(-tx, ty, tz), 2.0f, reflect));
			rt->addSceneObject(s1);
		}*/


		
		std::shared_ptr<SceneObject> cubemap(new CubeMap("cubemaps/SaintLazarusChurch3/"));
		rt->addSceneObject(cubemap);
		
		//std::shared_ptr<SceneObject> triangle(new Triangle(glm::vec3(-15.0, -10.0, -4.0), glm::vec3(15.0f, -10.0, -4),
		//glm::vec3(0, 10.0f, -15.0f), reflect));
		//rt->addSceneObject(triangle);
		
		std::shared_ptr<SceneObject> s1(new Sphere(glm::vec3(-3.0f, 0.0f, 3.0f), 2.0f, fresnel));
		rt->addSceneObject(s1);
		std::shared_ptr<SceneObject> s2(new Sphere(glm::vec3(1.0f, 0.0f, 2.0f), 2.0f, reflect));
		rt->addSceneObject(s2);
		//std::shared_ptr<SceneObject> s3(new Sphere(glm::vec3(0.0f, 3.0f, 0.0f), 2.0f, fresnel));
		//rt->addSceneObject(s3);

		//std::shared_ptr<SceneObject> s4(new Sphere(glm::vec3(-7.0f, -4.0f, 0.0f), 2.0f, fresnel));
		//rt->addSceneObject(s4);
		std::shared_ptr<SceneObject> s5(new Sphere(glm::vec3(6.0f, 2.0f, 0.0f), 3.0f, fresnel));
		rt->addSceneObject(s5);
		
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
