#include <iostream>
#include <fstream>
#include <sstream>

#include "glew/include/glew.h"

#include "Application.h"
#include "ResourceShader.h"
#include "Globals.h"

ResourceShader::ResourceShader(const char* path, const u64& id) : Resource()
{
	/*this->path = path;
	type = ResourceType::RESOURCE_SHADER;
	this->ID = id;

	renderer_id = -1;*/
}

ResourceShader::ResourceShader(const char* path)
{
	SetName(App->file_system->GetBaseFileName(path).c_str());
	this->path = std::string(path);
	type = ResourceType::RESOURCE_SHADER;
}

ResourceShader::~ResourceShader()
{
	glDeleteProgram(uniform_data.shader_id);
}

bool ResourceShader::LoadMemory()
{
	ParseAndCreateShader();

	return true;
}

void ResourceShader::FreeMemory()
{
	glDeleteProgram(uniform_data.shader_id);
}

bool ResourceShader::CreateMetaData(const u64& force_id)
{
	if (force_id == 0)
		ID = App->resources->GetRandomID();
	else
		ID = force_id;

	std::string alien_path = std::string(App->file_system->GetPathWithoutExtension(path) + "_meta.alien").data();
	
	JSON_Value* alien_value = json_value_init_object();
	JSON_Object* alien_object = json_value_get_object(alien_value);
	json_serialize_to_file_pretty(alien_value, alien_path.data());

	if (alien_value != nullptr && alien_object != nullptr) {
		JSONfilepack* alien = new JSONfilepack(alien_path, alien_object, alien_value);
		alien->StartSave();
		alien->SetString("Meta.ID", std::to_string(ID));
		alien->FinishSave();
		delete alien;
	}

	meta_data_path = std::string(LIBRARY_SHADERS_FOLDER + std::to_string(ID) + ".shader");
	std::string ext;
	App->file_system->SplitFilePath(path.data(), nullptr, nullptr, &ext);
	if (App->StringCmp(ext.data(), "shader"))
	{
		App->file_system->Copy(path.data(), meta_data_path.data());
	}
	else
	{
		// ...?
	}

	App->resources->AddResource(this);

	return true;
}

bool ResourceShader::DeleteMetaData()
{
	// TODO
	return true;
}

bool ResourceShader::ReadBaseInfo(const char* assets_path)
{
	bool ret = true;

	this->path = assets_path;
	std::string alien_path = App->file_system->GetPathWithoutExtension(path) + "_meta.alien";

	this->name = App->file_system->GetBaseFileName(path.c_str());

	JSON_Value* value = json_parse_file(alien_path.data());
	JSON_Object* object = json_value_get_object(value);

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* meta = new JSONfilepack(alien_path, object, value);

		ID = std::stoull(meta->GetString("Meta.ID"));

		delete meta;
	}

	meta_data_path = LIBRARY_SHADERS_FOLDER + std::to_string(ID) + ".shader";

	if (!App->file_system->Exists(meta_data_path.data())) {
		return false;
	}

	struct stat fileMeta;
	struct stat fileAssets;

	if (stat(meta_data_path.c_str(), &fileMeta) == 0 && stat(path.c_str(), &fileAssets) == 0) {
		if (fileAssets.st_mtime > fileMeta.st_mtime) {
			remove(meta_data_path.data());
			App->file_system->Copy(path.data(), meta_data_path.data());
		}
	}

	App->resources->AddResource(this);

	return ret;
}

void ResourceShader::ReadLibrary(const char* meta_data)
{
	this->meta_data_path = meta_data;
	ID = std::stoull(App->file_system->GetBaseFileName(meta_data_path.data()));
	App->resources->AddResource(this);
}

uint ResourceShader::ParseAndCreateShader()
{
	SHADER_PROGRAM_SOURCE source = ParseShader(path);
	uniform_data.shader_id = CreateShader(source.vertex_source, source.fragment_source);

	return uniform_data.shader_id;
}

bool ResourceShader::ChangeTemplate()
{
	bool ret = false;
	static int prev_type = (int)uniform_data.type;
	ImGui::Combo("Select Shader", &prev_type, "Diffuse\0Wave\0Basic Lighting\0\0");
	{
		//change shader
		if (prev_type != (int)uniform_data.type)
		{
			uniform_data.type = (SHADER_TEMPLATE)prev_type;

			ret = true;
		}
	}
	return ret;
}

void ResourceShader::HierarchyUniforms()
{
	ImGui::Text("--Uniforms--");
	switch (uniform_data.type) 
	{
		case SHADER_TEMPLATE::DIFUSSE: {//difusse
			//ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "custom_color", (unsigned int)type);
			ImGui::ColorEdit3("Albedo", uniform_data.standardShaderProperties.diffuse_color.ptr(), ImGuiColorEditFlags_Float);
			break; }
		case SHADER_TEMPLATE::WAVE: {//wave
			/*ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "custom_color", (unsigned int)type);
			ImGui::ColorEdit3(" ", &used_shader->material.custom_color.r);*/

			ImGui::Spacing();

			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "velocity", (unsigned int)type); ImGui::SameLine();
			ImGui::InputFloat("## ", &uniform_data.waveShaderProperties.mult_time, 0, 0, 2);

			ImGui::Spacing();

			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "amplitude", (unsigned int)type); ImGui::SameLine();
			ImGui::InputFloat("##", &uniform_data.waveShaderProperties.amplitude, 0, 0, 2);

			break; }

		case SHADER_TEMPLATE::BASIC_LIGHTING: {
			ImGui::ColorEdit3("Albedo", uniform_data.basicLightingShaderProperties.object_color.ptr(), ImGuiColorEditFlags_Float);

			ImGui::ColorEdit3("Lighting Color", uniform_data.basicLightingShaderProperties.lightColor.ptr(), ImGuiColorEditFlags_Float);
			ImGui::DragFloat3("Lighting Position", uniform_data.basicLightingShaderProperties.lightPosition.ptr());
			ImGui::DragFloat("Ambient Light Strength", &uniform_data.basicLightingShaderProperties.ambient_strength, 0.1f, 0.0f, 1.0f);
			ImGui::DragFloat("Specular Light Strength", &uniform_data.basicLightingShaderProperties.specular_strength, 0.1f, 0.0f, 1.0f);

			break; }

		default:
			LOG_ENGINE("We currently don't support editing this type of uniform...");
			break;
	}
}

void ResourceShader::UpdateUniforms()
{
	switch (uniform_data.type) {
	case SHADER_TEMPLATE::DIFUSSE: {//difusse
		// Object Material Properties
		SetUniformFloat3("diffuse_color", uniform_data.standardShaderProperties.diffuse_color);

		break; }
	case SHADER_TEMPLATE::WAVE: {//wave
		SetUniform1f("mult_time", uniform_data.waveShaderProperties.mult_time);
		SetUniform1f("amplitude", uniform_data.waveShaderProperties.amplitude);
		break; }

	case SHADER_TEMPLATE::BASIC_LIGHTING: {

		SetUniformFloat3("objectColor", uniform_data.basicLightingShaderProperties.object_color);
		// Lighting
		SetUniformFloat3("lightColor", uniform_data.basicLightingShaderProperties.lightColor);

		SetUniformFloat3("lightPos", uniform_data.basicLightingShaderProperties.lightPosition);
		SetUniform1f("ambientStrength", uniform_data.basicLightingShaderProperties.ambient_strength);
		SetUniform1f("specularStrength", uniform_data.basicLightingShaderProperties.specular_strength);

		SetUniformFloat3("viewPos", App->renderer3D->scene_fake_camera->GetCameraPosition());

		break; }

	default:
		LOG_ENGINE("We currently don't support editing this type of uniform...");
		break;

	}
}

void ResourceShader::Bind() const
{
	glUseProgram(uniform_data.shader_id);
}

void ResourceShader::Unbind() const
{
	glUseProgram(NULL);
}

void ResourceShader::SetUniform1i(const std::string& name, const int& value)
{
	glUniform1i(GetUniformLocation(name), value);
}

void ResourceShader::SetUniform1f(const std::string& name, const float& value)
{
	glUniform1f(GetUniformLocation(name), value);
}

void ResourceShader::SetUniformFloat3(const std::string& name, const float3& vec)
{
	glUniform3f(GetUniformLocation(name), vec.x, vec.y, vec.z);
}

void ResourceShader::SetUniform4f(const std::string& name, const float& v0, const float& v1, const float& v2, const float& v3)
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void ResourceShader::SetUniform4f(const std::string& name, const float4& vec)
{
	glUniform4f(GetUniformLocation(name), vec.x, vec.y, vec.z, vec.w);
}

void ResourceShader::SetUniformMat4f(const std::string& name, const math::float4x4& matrix)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, matrix.ptr());
}

SHADER_PROGRAM_SOURCE ResourceShader::ParseShader(const std::string& path)
{
	std::ifstream stream(path);

	std::string line;
	std::stringstream ss[2]; // one for each shader type

	SHADER_TYPE shader_type = SHADER_TYPE::UNKNOWN;

	while (getline(stream, line))
	{
		if (line.find("shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				shader_type = SHADER_TYPE::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				shader_type = SHADER_TYPE::FRAGMENT;
			}
		}
		else
		{
			ss[(int)shader_type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

uint ResourceShader::CreateShader(const std::string& vertex_shader, const std::string& fragment_shader)
{
	uint program = glCreateProgram();

	uint vertex_s = CompileShader(GL_VERTEX_SHADER, vertex_shader);
	uint fragment_s = CompileShader(GL_FRAGMENT_SHADER, fragment_shader);

	glAttachShader(program, vertex_s);
	glAttachShader(program, fragment_s);

	glLinkProgram(program);
	int status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		LOG_ENGINE("Shader was not linked successfully...");
	}

	glValidateProgram(program);

	glDetachShader(program, vertex_s);
	glDetachShader(program, fragment_s);

	glDeleteShader(vertex_s);
	glDeleteShader(fragment_s);

	if (vertex_s == 0 || fragment_s == 0)
		return 0u;

	return program;
}

uint ResourceShader::CompileShader(const uint& shader_type, const std::string& shader_source)
{
	uint shader_id = glCreateShader(shader_type);

	const char* source = shader_source.c_str();
	glShaderSource(shader_id, 1, &source, nullptr);
	glCompileShader(shader_id);

	int result;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(shader_id, length, &length, message);

		std::string type_string;
		switch (shader_type)
		{
		case GL_VERTEX_SHADER:
		{
			type_string = "vertex";
		}
		break;
		case GL_FRAGMENT_SHADER:
		{
			type_string = "fragment";
		}
		break;
		default:
			type_string = "WE_NULL";
			break;
		}

		LOG_ENGINE("ERROR: Can't compile %s shader...", type_string.c_str());
		LOG_ENGINE(message);

		glDeleteShader(shader_id);

		return 0u;
	}

	return shader_id;
}

int ResourceShader::GetUniformLocation(const std::string& name)
{
	if (uniform_location_cache.find(name) != uniform_location_cache.end())
		return uniform_location_cache[name];

	int location = glGetUniformLocation(uniform_data.shader_id, name.c_str());
	if (location == -1)
	{
		LOG_ENGINE("WARNING: Uniform %s doesn't exist...", name.c_str());
	}

	uniform_location_cache[name] = location;

	return location;
}

void ResourceShader::CreateShaderDoc(const int& type, const char* name)
{
	std::string file_output = std::string(SHADERS_FOLDER + std::string(name) + std::string(".shader"));
	switch (type) {
	case 1: { // class
	
			App->file_system->Copy(std::string(SHADERS_FOLDER + std::string("default.shader")).c_str(), file_output.data());
		break; }
	default:
		break;
	}

	// change de .h
	std::ifstream file(file_output);
	std::string file_str;
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			while (line.find("DataAlienTypeName") != std::string::npos) {
				line.replace(line.find("DataAlienTypeName"), 17, std::string(name));
			}
			if (file_str.empty()) {
				file_str = line;
			}
			else {
				file_str += std::string("\n") + line;
			}
		}
		file.close();
	}
	App->file_system->Save(file_output.data(), file_str.data(), file_str.size());
}