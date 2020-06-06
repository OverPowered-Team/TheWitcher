#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include <vector>
#include <string>

#include "Globals.h"
class ResourceTexture;

struct Cubemap
{
	enum SKYBOX_POS
	{
		POSITIVE_X,
		NEGATIVE_X,
		POSITIVE_Y,
		NEGATIVE_Y,
		POSITIVE_Z,
		NEGATIVE_Z,
	};

	Cubemap();

	std::string path_pos[6];

	void Reset()
	{
		for (int i = 0; i < 6; i++)
		{
			path_pos[i].clear();
		}
	}

	std::vector<std::string> ToVector()
	{
		std::vector<std::string> faces;
		for (int i = 0; i < 6; i++)
		{
			faces.push_back(path_pos[i]);
		}
		return faces;
	}

	ResourceTexture* skybox_textures[6] = { nullptr,nullptr, nullptr, nullptr, nullptr, nullptr };

	static std::string EnumToString(int i)
	{
		switch (i)
		{
		case SKYBOX_POS::POSITIVE_X:
		{
			return std::string("Positive X");
			break;
		}
		case SKYBOX_POS::NEGATIVE_X:
		{
			return std::string("Negative X");
			break;
		}
		case SKYBOX_POS::POSITIVE_Y:
		{
			return std::string("Positive Y");
			break;
		}
		case SKYBOX_POS::NEGATIVE_Y:
		{
			return std::string("Negative X");
			break;
		}
		case SKYBOX_POS::POSITIVE_Z:
		{
			return std::string("Positive Z");
			break;
		}
		case SKYBOX_POS::NEGATIVE_Z:
		{
			return std::string("Negative X");
			break;
		}
		default:
			break;
		}
	}

};

class Skybox
{
public:
	Skybox(Cubemap* cubemap);
	~Skybox();

	uint GenereteCubeMapFromTextures(ResourceTexture* skybox_textures[6]);
	void SetBuffers();
	void ChangeTextureByType(Cubemap::SKYBOX_POS type, const uint& id_texture, const uint& width, const uint& height);
	void ChangePositiveX(const uint& id_texture, const uint& width, const uint& height);
	void ChangeNegativeX(const uint& id_texture, const uint& width, const uint& height);
	void ChangePositiveY(const uint& id_texture, const uint& width, const uint& height);
	void ChangeNegativeY(const uint& id_texture, const uint& width, const uint& height);
	void ChangePositiveZ(const uint& id_texture, const uint& width, const uint& height);
	void ChangeNegativeZ(const uint& id_texture, const uint& width, const uint& height);

public:

	std::vector<std::string> textures[6];
	uint vao = 0;
	uint vbo = 0;
	uint skybox_texture = 0;

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