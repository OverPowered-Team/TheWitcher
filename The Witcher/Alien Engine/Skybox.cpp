#include "glew/include/glew.h"
#include "stb_image.h"

#include "Skybox.h"
#include "Application.h"
#include "ModuleImporter.h"
#include "ResourceTexture.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"

#include "Optick/include/optick.h"

Skybox::Skybox()
{

}

Skybox::~Skybox()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

uint Skybox::LoadCubeMap(const std::vector<std::string>& texture_files)
{
	OPTICK_EVENT();

	GLuint texture_id;
	glGenTextures(1, &texture_id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
	
	for (int i = 0; i < texture_files.size(); ++i)
	{
		std::string path = App->file_system->GetPathWithoutExtension(texture_files[i]);
		path += "_meta.alien";

		u64 ID = App->resources->GetIDFromAlienPath(path.data());
		ResourceTexture* t = (ResourceTexture*)App->resources->GetResourceWithID(ID);
		if(t->references == 0)
			t->IncreaseReferences();

		glBindTexture(GL_TEXTURE_2D, t->id);
		unsigned char* data = new unsigned char[sizeof(char) * t->width * t->height * 4];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, t->width, t->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			LOG_ENGINE("Cubemap texture couldn't be loaded: %s", texture_files[i].c_str());
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		RELEASE_ARRAY(data);
	}

	return texture_id;
}

uint Skybox::LoadCubeMapFromLibraryFiles(const std::vector<std::string>& texture_files)
{
	OPTICK_EVENT();

	GLuint texture_id;
	glGenTextures(1, &texture_id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

	for (int i = 0; i < texture_files.size(); ++i)
	{
		std::string name_id = App->file_system->GetBaseFileName(texture_files[i].c_str());
		u64 ID = std::stoull(name_id);
		ResourceTexture* t = (ResourceTexture*)App->resources->GetResourceWithID(ID);
		if(t != nullptr && t->references == 0)
			t->IncreaseReferences();
		if (t != nullptr)
		{
			glBindTexture(GL_TEXTURE_2D, t->id);
			unsigned char* data = new unsigned char[sizeof(char) * t->width * t->height * 4];
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, t->width, t->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}
			else
			{
				LOG_ENGINE("Cubemap texture couldn't be loaded: %s", texture_files[i].c_str());
			}

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			RELEASE_ARRAY(data);
		}
	}

	return texture_id;
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
}


void Skybox::ChangePositiveX(const uint& id_skybox, const uint& id_texture, const uint& width, const uint& height)
{
	OPTICK_EVENT();

	glBindTexture(GL_TEXTURE_2D, id_texture);
	unsigned char* data = new unsigned char[sizeof(char) * width * height * 4];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id_skybox);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	RELEASE_ARRAY(data);
}

void Skybox::ChangeNegativeX(const uint& id_skybox, const uint& id_texture, const uint& width, const uint& height)
{
	OPTICK_EVENT();

	glBindTexture(GL_TEXTURE_2D, id_texture);
	unsigned char* data = new unsigned char[sizeof(char) * width * height * 4];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id_skybox);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	RELEASE_ARRAY(data);
}

void Skybox::ChangePositiveY(const uint& id_skybox, const uint& id_texture, const uint& width, const uint& height)
{
	OPTICK_EVENT();

	glBindTexture(GL_TEXTURE_2D, id_texture);
	unsigned char* data = new unsigned char[sizeof(char) * width * height * 4];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id_skybox);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	RELEASE_ARRAY(data);
}

void Skybox::ChangeNegativeY(const uint& id_skybox, const uint& id_texture, const uint& width, const uint& height)
{
	OPTICK_EVENT();

	glBindTexture(GL_TEXTURE_2D, id_texture);
	unsigned char* data = new unsigned char[sizeof(char) * width * height * 4];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id_skybox);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	RELEASE_ARRAY(data);
}

void Skybox::ChangePositiveZ(const uint& id_skybox, const uint& id_texture, const uint& width, const uint& height)
{
	OPTICK_EVENT();

	glBindTexture(GL_TEXTURE_2D, id_texture);
	unsigned char* data = new unsigned char[sizeof(char) * width * height * 4];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id_skybox);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	
	RELEASE_ARRAY(data);
}

void Skybox::ChangeNegativeZ(const uint& id_skybox, const uint& id_texture, const uint& width, const uint& height)
{
	OPTICK_EVENT();

	glBindTexture(GL_TEXTURE_2D, id_texture);
	unsigned char* data = new unsigned char[sizeof(char) * width * height * 4];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id_skybox);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	RELEASE_ARRAY(data);
}
