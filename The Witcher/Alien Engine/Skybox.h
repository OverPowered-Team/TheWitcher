#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include <vector>
#include <string>

#include "Globals.h"

struct Cubemap
{
	std::string neg_z;
	std::string pos_z;
	std::string pos_y;
	std::string neg_y;
	std::string pos_x;
	std::string neg_x;

	void Reset()
	{
		neg_z.clear();
		pos_z.clear();
		pos_y.clear();
		neg_y.clear();
		pos_x.clear();
		neg_x.clear();
	}

	std::vector<std::string> ToVector()
	{
		std::vector<std::string> faces;

		faces.push_back(neg_z);
		faces.push_back(pos_z);
		faces.push_back(pos_y);
		faces.push_back(neg_y);
		faces.push_back(pos_x);
		faces.push_back(neg_x);

		return faces;
	}
};

class Skybox
{
public:
	Skybox();
	~Skybox();

	uint LoadCubeMap(const std::vector<std::string>& texture_files);
	void SetBuffers();

	void ChangeNegativeZ(const uint& id, const char* path);
	void ChangePositiveZ(const uint& id, const char* path);
	void ChangePositiveY(const uint& id, const char* path);
	void ChangeNegativeY(const uint& id, const char* path);
	void ChangePositiveX(const uint& id, const char* path);
	void ChangeNegativeX(const uint& id, const char* path);

public:
	std::vector<std::string> textures[6];
	uint vao, vbo;

private:
	float vertices[108] = {
		// positions          
		-10000.0f,  10000.0f, -10000.0f,
		-10000.0f, -10000.0f, -10000.0f,
		 10000.0f, -10000.0f, -10000.0f,
		 10000.0f, -10000.0f, -10000.0f,
		 10000.0f,  10000.0f, -10000.0f,
		-10000.0f,  10000.0f, -10000.0f,
			 		   			 
		-10000.0f, -10000.0f,  10000.0f,
		-10000.0f, -10000.0f, -10000.0f,
		-10000.0f,  10000.0f, -10000.0f,
		-10000.0f,  10000.0f, -10000.0f,
		-10000.0f,  10000.0f,  10000.0f,
		-10000.0f, -10000.0f,  10000.0f,
			 		   			 
		 10000.0f, -10000.0f, -10000.0f,
		 10000.0f, -10000.0f,  10000.0f,
		 10000.0f,  10000.0f,  10000.0f,
		 10000.0f,  10000.0f,  10000.0f,
		 10000.0f,  10000.0f, -10000.0f,
		 10000.0f, -10000.0f, -10000.0f,
			 		   			 
		-10000.0f, -10000.0f,  10000.0f,
		-10000.0f,  10000.0f,  10000.0f,
		 10000.0f,  10000.0f,  10000.0f,
		 10000.0f,  10000.0f,  10000.0f,
		 10000.0f, -10000.0f,  10000.0f,
		-10000.0f, -10000.0f,  10000.0f,
			 		   			 
		-10000.0f,  10000.0f, -10000.0f,
		 10000.0f,  10000.0f, -10000.0f,
		 10000.0f,  10000.0f,  10000.0f,
		 10000.0f,  10000.0f,  10000.0f,
		-10000.0f,  10000.0f,  10000.0f,
		-10000.0f,  10000.0f, -10000.0f,
			 		   			 
		-10000.0f, -10000.0f, -10000.0f,
		-10000.0f, -10000.0f,  10000.0f,
		 10000.0f, -10000.0f, -10000.0f,
		 10000.0f, -10000.0f, -10000.0f,
		-10000.0f, -10000.0f,  10000.0f,
		 10000.0f, -10000.0f,  10000.0f
	};
};

#endif /* __SKYBOX_H__ */