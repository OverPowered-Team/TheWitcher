#include "glew/include/glew.h"
#include "stb_image.h"

#include "Skybox.h"
#include "Application.h"
#include "ModuleImporter.h"
#include "ResourceTexture.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"

#include "Optick/include/optick.h"
#include "mmgr/mmgr.h"

Cubemap::Cubemap()
{
	for (int i = 0; i < 6; i++)
	{
		skybox_textures[i] = App->resources->default_skybox_textures[i];
	}

	for (int i = 0; i < 6; i++)
	{
		path_pos[i] = "Default";
	}
}

Skybox::Skybox(Cubemap* cubemap)
{
	glGenTextures(1, &skybox_texture);
	// Set created texture type to Cubemap
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	GenereteCubeMapFromTextures(cubemap->skybox_textures);
	SetBuffers();
}

Skybox::~Skybox()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteTextures(1, &skybox_texture);
}

uint Skybox::GenereteCubeMapFromTextures(ResourceTexture* skybox_textures[6])
{
	OPTICK_EVENT();

	for (int i = 0; i < 6; ++i)
	{
		ResourceTexture* t = skybox_textures[i];
		
		if (t != nullptr)
		{

			glBindTexture(GL_TEXTURE_2D, t->id);
			unsigned char* data = new unsigned char[sizeof(char) * t->width * t->height * 4];
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, t->width, t->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}
			else
			{
				LOG_ENGINE("Cubemap texture couldn't be loaded: %s", t->GetAssetsPath());
			}

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

			RELEASE_ARRAY(data);
		}
	}

	return 0;
}



void Skybox::SetBuffers()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Skybox::ChangeTextureByType(Cubemap::SKYBOX_POS type, const uint& id_texture, const uint& width, const uint& height)
{
	switch (type)
	{
	case Cubemap::POSITIVE_X:
		ChangePositiveX(id_texture, width, height);
		break;
	case Cubemap::NEGATIVE_X:
		ChangeNegativeX(id_texture, width, height);
		break;
	case Cubemap::POSITIVE_Y:
		ChangePositiveY(id_texture, width, height);
		break;
	case Cubemap::NEGATIVE_Y:
		ChangeNegativeY(id_texture, width, height);
		break;
	case Cubemap::POSITIVE_Z:
		ChangePositiveZ(id_texture, width, height);
		break;
	case Cubemap::NEGATIVE_Z:
		ChangeNegativeZ(id_texture, width, height);
		break;
	default:
		break;
	}
}


void Skybox::ChangePositiveX(const uint& id_texture, const uint& width, const uint& height)
{
	OPTICK_EVENT();

	glBindTexture(GL_TEXTURE_2D, id_texture);
	unsigned char* data = new unsigned char[sizeof(char) * width * height * 4];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	RELEASE_ARRAY(data);
}

void Skybox::ChangeNegativeX(const uint& id_texture, const uint& width, const uint& height)
{
	OPTICK_EVENT();

	glBindTexture(GL_TEXTURE_2D, id_texture);
	unsigned char* data = new unsigned char[sizeof(char) * width * height * 4];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	RELEASE_ARRAY(data);
}

void Skybox::ChangePositiveY(const uint& id_texture, const uint& width, const uint& height)
{
	OPTICK_EVENT();

	glBindTexture(GL_TEXTURE_2D, id_texture);
	unsigned char* data = new unsigned char[sizeof(char) * width * height * 4];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	RELEASE_ARRAY(data);
}

void Skybox::ChangeNegativeY(const uint& id_texture, const uint& width, const uint& height)
{
	OPTICK_EVENT();

	glBindTexture(GL_TEXTURE_2D, id_texture);
	unsigned char* data = new unsigned char[sizeof(char) * width * height * 4];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	RELEASE_ARRAY(data);
}

void Skybox::ChangePositiveZ(const uint& id_texture, const uint& width, const uint& height)
{
	OPTICK_EVENT();

	glBindTexture(GL_TEXTURE_2D, id_texture);
	unsigned char* data = new unsigned char[sizeof(char) * width * height * 4];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	
	RELEASE_ARRAY(data);
}

void Skybox::ChangeNegativeZ(const uint& id_texture, const uint& width, const uint& height)
{
	OPTICK_EVENT();

	glBindTexture(GL_TEXTURE_2D, id_texture);
	unsigned char* data = new unsigned char[sizeof(char) * width * height * 4];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	RELEASE_ARRAY(data);
}
