#ifndef _CUBEMAP_HPP__
#define _CUBEMAP_HPP__

#include <string>
#include <limits>
#include <sstream>

#include <glm/glm.hpp>

#include <IL/il.h>
#include <IL/ilu.h>


class CubeMap : public SceneObject {
public:
	CubeMap(std::string cubemap_path) {
		ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
		loadImage(cubemap_path+"posx.jpg", this->posx);
		loadImage(cubemap_path+"negx.jpg", this->negx);
		loadImage(cubemap_path+"posy.jpg", this->posy);
		loadImage(cubemap_path+"negy.jpg", this->negy);
		loadImage(cubemap_path+"posz.jpg", this->posz);
		loadImage(cubemap_path+"negz.jpg", this->negz);
	}
	
	/**
	  * Ray-trace function that returns what texel you hit in the
	  * cube map, since any ray will hit some point in the cube map
	  */
	glm::vec3 rayTrace(Ray &ray, const float& t, RayTracerState& state) {
		glm::vec3 out_color(0.0f);
		glm::vec3 dir =  ray.getDirection();
		float x = dir.x;//std::abs(dir.x);
		float y = dir.y;//std::abs(dir.y);
		float z = dir.z;//std::abs(dir.z);
		if ((std::abs(dir.x) >= std::abs(dir.y)) && (std::abs(dir.x) >= std::abs(dir.z))){
 			if (dir.x > 0.0f){
				/*		float t = z/x;
				float s = y/x;*/

				float s = 0.5f-0.5f*z/x;
				float t = 0.5f-0.5f*y/x;
				out_color = readTexture(posx, s, t);
			}
			else{
				float s = 0.5f-0.5f*z/x;
				float t =  0.5f*y/x-0.5f+1;
				out_color = readTexture(negx, s, t);
			}
		}
		else if ((std::abs(dir.y) >= std::abs(dir.x)) && (std::abs(dir.y) >= std::abs(dir.z))){
			if (dir.y > 0.0f){
				float s =  0.5f*x/y-0.5f+1;
				float t =  0.5f*z/y-0.5f+1;
				out_color = readTexture(posy, s, t);
			}
			else{
				float s = 0.5f-0.5f*x/y;
				float t =  0.5f*z/y-0.5f+1;
				out_color = readTexture(negy, s, t);
			}
		}
		else{
			if (dir.z > 0.0f){
				float t = 0.5f-0.5f*y/z;
				float s =  0.5f*x/z-0.5f+1;
				out_color = readTexture(posz, s, t);
			}
			else{
				float s =  0.5f*x/z-0.5f+1;
				float t =  (0.5f*y/z-0.5f)+1;
				out_color = readTexture(negz, abs(s), abs(t));
			}
		}
		return out_color;
	}
	
	/**
	  * A ray will always hit the cube map by definition, but the point of intersection
	  * is as far away as possible
	  */
	float intersect(const Ray& r) {
		return std::numeric_limits<float>::max();
	}

private:
	struct texture {
		std::vector<float> data;
		unsigned int width;
		unsigned int height;
	};

	/**
	  * Returns the texel at texture coordinate [s, t] in texture tex
	  */
	static glm::vec3 readTexture(texture& tex, float s, float t) {
		glm::vec3 out_color;

		float xf = std::min(s*tex.width, tex.width-1.0f);
		float yf = std::min(t*tex.height, tex.height-1.0f);

		unsigned int xm = static_cast<unsigned int>(xf);
		unsigned int ym = static_cast<unsigned int>(yf);

		unsigned int i0 = (ym*tex.width + xm)*3;

		for (int k=0; k<3; ++k) {
			float c0 = tex.data.at(i0+k);
			out_color[k] = c0;
		}

		return out_color;
	}
	//glm::vec3 readTexture(float u, float v, int sizeU, int sizeV)
	//{
	//	u = abs(u);
	//	v = abs(v);
	//	int umin = int(sizeU * u);
	//	int vmin = int(sizeV * v);
	//	int umax = int(sizeU * u) + 1;
	//	int vmax = int(sizeV * v) + 1;
	//	float ucoef = fabsf(sizeU * u - umin);
	//	float vcoef = fabsf(sizeV * v - vmin);
	//
	//	// The texture is being addressed on [0,1]
	//	// There should be an addressing type in order to 
	//	// determine how we should access texels when
	//	// the coordinates are beyond those boundaries.

	//	// Clamping is our current default and the only
	//	// implemented addressing type for now.
	//	// Clamping is done by bringing anything below zero
	//	// to the coordinate zero
	//	// and everything beyond one, to one.
	//	umin = glm::min(glm::max(umin, 0), sizeU - 1);
	//	umax = glm::min(glm::max(umax, 0), sizeU - 1);
	//	vmin = glm::min(glm::max(vmin, 0), sizeV - 1);
	//	vmax = glm::min(glm::max(vmax, 0), sizeV - 1);

	//	// What follows is a bilinear interpolation
	//	// along two coordinates u and v.

	//	glm::vec3 output = glm::vec3( (1.0f - vcoef) * 
	//		((1.0f - ucoef) * tab[umin  + sizeU * vmin] 
	//	+ ucoef * tab[umax + sizeU * vmin])
	//		+   vcoef * 
	//		((1.0f - ucoef) * tab[umin  + sizeU * vmax] 

	//	+ ucoef * tab[umax + sizeU * vmax]));
	//	return output;
	//}
	/**
	  * Loads an image into memory from file
	  */
	static void loadImage(std::string filename, texture& tex) {
		ILuint ImageName;

		ilGenImages(1, &ImageName); // Grab a new image name.
		ilBindImage(ImageName); 
		
		if (!ilLoadImage(filename.c_str())) {
			ILenum e;
			std::stringstream error;
			while ((e = ilGetError()) != IL_NO_ERROR) {
				error << e << ": " << iluErrorString(e) << std::endl;
			}
			ilDeleteImages(1, &ImageName); // Delete the image name. 
			throw std::runtime_error(error.str());
		}

		tex.width = ilGetInteger(IL_IMAGE_WIDTH); // getting image width
		tex.height = ilGetInteger(IL_IMAGE_HEIGHT); // and height
		tex.data.resize(tex.width*tex.height*3);
		
		ilCopyPixels(0, 0, 0, tex.width, tex.height, 1, IL_RGB, IL_FLOAT, tex.data.data());
		ilDeleteImages(1, &ImageName); // Delete the image name. 
	}

	texture posx, negx, posy, negy, posz, negz;
};

#endif
