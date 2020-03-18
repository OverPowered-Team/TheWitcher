#include "glew/include/glew.h"
#include "stb_image.h"

#include "Skybox.h"
#include "Application.h"
#include "ModuleImporter.h"
#include "ResourceTexture.h"
#include "ModuleResources.h"

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
	GLuint texture_id;
	glGenTextures(1, &texture_id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
	
	int width, height, channels;
	for (int i = 0; i < texture_files.size(); ++i)
	{
		auto item = App->resources->resources.begin();
		for (; item != App->resources->resources.end(); ++item) {
			if ((*item)->GetType() == ResourceType::RESOURCE_TEXTURE && strcmp(texture_files[i].data(), (*item)->GetAssetsPath()) == 0) {
				(*item)->IncreaseReferences();
				break;
			}
		}

		unsigned char* data = stbi_load(texture_files[i].c_str(), &width, &height, &channels, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			LOG_ENGINE("Cubemap texture couldn't be loaded: %s", texture_files[i].c_str());
			stbi_image_free(data);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
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

void Skybox::ChangeNegativeZ(const uint& id, const char* path)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	int width, height, channels;
	unsigned char* data = stbi_load(path, &width, &height, &channels, 0);

	/*auto item = App->resources->resources.begin();
	for (; item != App->resources->resources.end(); ++item) {
		if ((*item)->GetType() == ResourceType::RESOURCE_TEXTURE && strcmp(path, (*item)->GetAssetsPath()) == 0) {
			(*item)->IncreaseReferences();
			break;
		}
	}*/
	
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::ChangePositiveZ(const uint& id, const char* path)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	int width, height, channels;
	unsigned char* data = stbi_load(path, &width, &height, &channels, 0);

	/*auto item = App->resources->resources.begin();
	for (; item != App->resources->resources.end(); ++item) {
		if ((*item)->GetType() == ResourceType::RESOURCE_TEXTURE && strcmp(path, (*item)->GetAssetsPath()) == 0) {
			(*item)->IncreaseReferences();
			break;
		}
	}*/

	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::ChangePositiveY(const uint& id, const char* path)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	int width, height, channels;
	unsigned char* data = stbi_load(path, &width, &height, &channels, 0);

	/*auto item = App->resources->resources.begin();
	for (; item != App->resources->resources.end(); ++item) {
		if ((*item)->GetType() == ResourceType::RESOURCE_TEXTURE && strcmp(path, (*item)->GetAssetsPath()) == 0) {
			(*item)->IncreaseReferences();
			break;
		}
	}*/

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::ChangeNegativeY(const uint& id, const char* path)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	int width, height, channels;
	unsigned char* data = stbi_load(path, &width, &height, &channels, 0);

	/*auto item = App->resources->resources.begin();
	for (; item != App->resources->resources.end(); ++item) {
		if ((*item)->GetType() == ResourceType::RESOURCE_TEXTURE && strcmp(path, (*item)->GetAssetsPath()) == 0) {
			(*item)->IncreaseReferences();
			break;
		}
	}*/

	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::ChangePositiveX(const uint& id, const char* path)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	int width, height, channels;
	unsigned char* data = stbi_load(path, &width, &height, &channels, 0);

	/*auto item = App->resources->resources.begin();
	for (; item != App->resources->resources.end(); ++item) {
		if ((*item)->GetType() == ResourceType::RESOURCE_TEXTURE && strcmp(path, (*item)->GetAssetsPath()) == 0) {
			(*item)->IncreaseReferences();
			break;
		}
	}*/

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::ChangeNegativeX(const uint& id, const char* path)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	int width, height, channels;
	unsigned char* data = stbi_load(path, &width, &height, &channels, 0);

	/*auto item = App->resources->resources.begin();
	for (; item != App->resources->resources.end(); ++item) {
		if ((*item)->GetType() == ResourceType::RESOURCE_TEXTURE && strcmp(path, (*item)->GetAssetsPath()) == 0) {
			(*item)->IncreaseReferences();
			break;
		}
	}*/

	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
