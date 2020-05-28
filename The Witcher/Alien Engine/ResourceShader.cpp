#include <iostream>
#include <fstream>
#include <sstream>

#include "glew/include/glew.h"

#include "Application.h"
#include "ResourceShader.h"
#include "ResourceMaterial.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "ModuleRenderer3D.h"
#include "Globals.h"
#include "Time.h"
#include "ComponentLightDirectional.h"
#include "ComponentLightSpot.h"
#include "ComponentTransform.h"
#include "ComponentLightPoint.h"
#include "Viewport.h"
#include "mmgr/mmgr.h"

#include "Optick/include/optick.h"

ResourceShader::ResourceShader()
{
	type = ResourceType::RESOURCE_SHADER;
}

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
	glDeleteProgram(shader_id);
}

bool ResourceShader::LoadMemory()
{
	ParseAndCreateShader();

	return true;
}

void ResourceShader::FreeMemory()
{
	glDeleteProgram(shader_id);
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
		JSONfilepack* alien = new JSONfilepack(alien_path.data(), alien_object, alien_value);
		alien->StartSave();
		alien->SetString("Meta.ID", std::to_string(ID).data());
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

	TryToSetShaderType();

	JSON_Value* value = json_parse_file(alien_path.data());
	JSON_Object* object = json_value_get_object(value);

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* meta = new JSONfilepack(alien_path.data(), object, value);

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

void ResourceShader::TryToSetShaderType()
{
	if (std::strcmp(name.c_str(), "default_shader") == 0)
		shaderType = SHADER_TEMPLATE::DEFAULT;
	else if (std::strcmp(name.c_str(), "shader_wave") == 0)
		shaderType = SHADER_TEMPLATE::WAVE;
	else if (std::strcmp(name.c_str(), "iluminated_shader") == 0)
		shaderType = SHADER_TEMPLATE::ILUMINATED;
	else if (std::strcmp(name.c_str(), "particle_shader") == 0)
		shaderType = SHADER_TEMPLATE::PARTICLE;
	else if (std::strcmp(name.c_str(), "simple_depth_shader") == 0)
		shaderType = SHADER_TEMPLATE::SHADOW;
	else if (std::strcmp(name.c_str(), "water_shader") == 0)
		shaderType = SHADER_TEMPLATE::WATER;
	else if (std::strcmp(name.c_str(), "shield_shader") == 0)
		shaderType = SHADER_TEMPLATE::SHIELD;
	else if (std::strcmp(name.c_str(), "shield_fresnel_shader") == 0)
		shaderType = SHADER_TEMPLATE::SHIELD_FRESNEL;
	else 
		shaderType = SHADER_TEMPLATE::NO_TEMPLATE;
}

uint ResourceShader::ParseAndCreateShader()
{
	SHADER_PROGRAM_SOURCE source = ParseShader(meta_data_path);
	shader_id = CreateShader(source.vertex_source, source.fragment_source);

	return shader_id;
}

SHADER_TEMPLATE ResourceShader::GetShaderType() const
{
	return shaderType;
}

void ResourceShader::UpdateUniforms(ShaderInputs inputs)
{
	OPTICK_EVENT();

	switch (shaderType) {
	case SHADER_TEMPLATE::DEFAULT: {
		SetUniform4f("objectMaterial.diffuse_color", inputs.standardShaderProperties.diffuse_color);
		SetUniform1f("objectMaterial.smoothness", inputs.standardShaderProperties.smoothness);
		SetUniform1f("objectMaterial.metalness", inputs.standardShaderProperties.metalness);
		break; }

	case SHADER_TEMPLATE::WAVE: {
		SetUniform1f("mult_time", inputs.waveShaderProperties.mult_time);
		SetUniform1f("amplitude", inputs.waveShaderProperties.amplitude);
		break; }

	case SHADER_TEMPLATE::ILUMINATED: {
		SetUniformFloat3("diffuse_color", inputs.iluminatedShaderProperties.object_color);
		break; }

	case SHADER_TEMPLATE::PARTICLE: {
		SetUniform4f("objectMaterial.diffuse_color", inputs.particleShaderProperties.color);
		break; }
	case SHADER_TEMPLATE::SHADOW: {

		break; }

	case SHADER_TEMPLATE::WATER: {
		SetUniform4f("objectMaterial.diffuse_color", inputs.standardShaderProperties.diffuse_color);
		SetUniform1f("objectMaterial.smoothness", inputs.standardShaderProperties.smoothness);
		SetUniform1f("objectMaterial.metalness", inputs.standardShaderProperties.metalness);
		SetUniform1i("reflection_texture", 1);
		SetUniform1i("refraction_texture", 2);
		SetUniform1i("dudv_map", 3);
		SetUniform1f("move_factor", Time::GetTimeSinceStart() * 0.075f);
		SetUniformFloat3("camera_position", App->renderer3D->actual_game_camera->GetCameraPosition());
		ApplyLightsUniforms();
		break; }

	case SHADER_TEMPLATE::SHIELD: {
		SetUniformFloat3("objectMaterial.diffuse_color", inputs.shieldShaderProperties.color);
		SetUniformFloat3("hitPosition", inputs.shieldShaderProperties.hit_position);
		
		break; }

	case SHADER_TEMPLATE::SHIELD_FRESNEL: {
		SetUniform4f("diffuse_color", inputs.shieldFresnelShaderProperties.color);
		SetUniform1f("shieldStrength", inputs.shieldFresnelShaderProperties.shieldStrength);
		SetUniform1f("shieldCooldown", inputs.shieldFresnelShaderProperties.shieldCooldown);
		SetUniform1f("exponent", inputs.shieldFresnelShaderProperties.fresnel_exponent);
		//SetUniform1i("numHits", inputs.shieldFresnelShaderProperties.numHits);
		break; }

	default:
		LOG_ENGINE("We currently don't support editing this type of uniform...");
		break;

	}
}

void ResourceShader::ApplyCurrentShaderGlobalUniforms(ComponentCamera* camera)
{
	switch (shaderType)
	{
	case SHADER_TEMPLATE::DEFAULT:
		SetUniformMat4f("view", camera->GetViewMatrix4x4());
		SetUniformMat4f("projection", camera->GetProjectionMatrix4f4());
		SetUniformFloat3("view_pos", camera->GetCameraPosition());
		SetUniform1i("activeFog", camera->activeFog);
		if (camera->activeFog)
		{
			SetUniformFloat3("backgroundColor", float3(camera->camera_color_background.r, camera->camera_color_background.g, camera->camera_color_background.b));
			SetUniform1f("density", camera->fogDensity);
			SetUniform1f("gradient", camera->fogGradient);
		}
		ApplyLightsUniforms();
		break;

	case SHADER_TEMPLATE::PARTICLE:
		SetUniformMat4f("view", camera->GetViewMatrix4x4());
		SetUniformMat4f("projection", camera->GetProjectionMatrix4f4());
		SetUniform1i("activeFog", camera->activeFog);
		if (camera->activeFog)
		{
			SetUniformFloat3("backgroundColor", float3(camera->camera_color_background.r, camera->camera_color_background.g, camera->camera_color_background.b));
			SetUniform1f("density", camera->fogDensity);
			SetUniform1f("gradient", camera->fogGradient);
		}
		break;

	case SHADER_TEMPLATE::SHIELD_FRESNEL:
		SetUniformMat4f("view", camera->GetViewMatrix4x4());
		SetUniformMat4f("projection", camera->GetProjectionMatrix4f4());
		SetUniform1i("activeFog", camera->activeFog);
		if (camera->activeFog)
		{
			SetUniformFloat3("backgroundColor", float3(camera->camera_color_background.r, camera->camera_color_background.g, camera->camera_color_background.b));
			SetUniform1f("density", camera->fogDensity);
			SetUniform1f("gradient", camera->fogGradient);
		}
		break;

	}
}

void ResourceShader::ApplyLightsUniforms()
{
	OPTICK_EVENT();

	// Light uniforms set from here
	SetUniform3i("max_lights", App->objects->GetNumOfDirLights(), App->objects->GetNumOfPointLights(), App->objects->GetNumOfSpotLights());

	if (App->objects->directional_light_properites.size() > 0)
		SetDirectionalLights("dir_light", App->objects->directional_light_properites);
	if (App->objects->point_light_properites.size() > 0)
		SetPointLights("point_light", App->objects->point_light_properites);
	if (App->objects->spot_light_properites.size() > 0)
		SetSpotLights("spot_light", App->objects->spot_light_properites);
}

void ResourceShader::Bind() const
{
	OPTICK_EVENT();

	glUseProgram(shader_id);
}

void ResourceShader::Unbind() const
{
	OPTICK_EVENT();

	glUseProgram(NULL);
}

void ResourceShader::SetUniform1i(const std::string& name, const int& value)
{
	OPTICK_EVENT();

	int location = GetUniformLocation(name);
	if (location != -1)
		glUniform1i(location, value);
}

void ResourceShader::SetUniform1ui(const std::string& name, const uint& value)
{
	OPTICK_EVENT();

	int location = GetUniformLocation(name);
	if(location != -1)
		glUniform1ui(GetUniformLocation(name), value);
}

void ResourceShader::SetUniform1f(const std::string& name, const float& value)
{
	OPTICK_EVENT();

	int location = GetUniformLocation(name);
	if (location != -1)
		glUniform1f(location, value);
}

void ResourceShader::SetUniform3i(const std::string& name, const int& v0, const int& v1, const int& v2)
{
	OPTICK_EVENT();

	int location = GetUniformLocation(name);
	if (location != -1)
		glUniform3i(GetUniformLocation(name), v0, v1, v2);
}

void ResourceShader::SetUniformFloat3(const std::string& name, const float3& vec)
{
	OPTICK_EVENT();

	int location = GetUniformLocation(name);
	if (location != -1)
		glUniform3f(location, vec.x, vec.y, vec.z);
}

void ResourceShader::SetUniformFloat3v(const std::string& name, const float3* vec, uint count)
{
	OPTICK_EVENT();

	int location = GetUniformLocation(name);
	if (location != -1)
		glUniform3fv(location, count, vec[0].ptr());
}

void ResourceShader::SetUniformFloatv(const std::string& name, const float* vec, uint count)
{
	OPTICK_EVENT();

	int location = GetUniformLocation(name);
	if (location != -1)
		glUniform1fv(location,count, vec);
}

void ResourceShader::SetUniformIntv(const std::string& name, const int* vec, uint count)
{
	OPTICK_EVENT();

	int location = GetUniformLocation(name);
	if (location != -1)
		glUniform1iv(location, count, vec);
}

void ResourceShader::SetUniform4f(const std::string& name, const float& v0, const float& v1, const float& v2, const float& v3)
{
	OPTICK_EVENT();

	int location = GetUniformLocation(name);
	if (location != -1)
		glUniform4f(location, v0, v1, v2, v3);
}

void ResourceShader::SetUniform4f(const std::string& name, const float4& vec)
{
	OPTICK_EVENT();

	int location = GetUniformLocation(name);
	if (location != -1)
		glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
}

void ResourceShader::SetUniformMat4f(const std::string& name, const math::float4x4& matrix)
{
	OPTICK_EVENT();

	int location = GetUniformLocation(name);
	if (location != -1)
		glUniformMatrix4fv(location, 1, GL_FALSE, matrix.ptr());
}

void ResourceShader::SetUniformMat4f(const std::string& name, const math::float4x4* matrix, uint count)
{
	OPTICK_EVENT();

	int location = GetUniformLocation(name);
	if (location != -1)
		glUniformMatrix4fv(location, count, GL_TRUE, matrix[0].ptr());
}

void ResourceShader::SetDirectionalLights(const std::string& name, const std::list<DirLightProperties*>& dirLights)
{
	OPTICK_EVENT();

	int i = 0;
	std::string tmp_name(name.c_str());
	tmp_name.append("[%i]");

	std::string clightSpaceMatrix("lightSpaceMatrix");
	clightSpaceMatrix.append("[%i]");

	std::string clightSpaceMatrixBaked("lightSpaceMatrixBaked");
	clightSpaceMatrixBaked.append("[%i]");

	SetUniform1i("num_space_matrix", dirLights.size());
	for (std::list<DirLightProperties*>::const_iterator iter = dirLights.begin(); iter != dirLights.end(); iter++)
	{
		if ((*iter)->light->game_object_attached->IsEnabled() && (*iter)->enabled)
		{
			char cname[128];
			sprintf_s(cname, tmp_name.c_str(), i);

			std::string cintensity = std::string(cname).append(".intensity");
			SetUniform1f(cintensity, (*iter)->intensity);

			// Variables Array 
			float3 variablesVec3[5] = { (*iter)->position,(*iter)->ambient, (*iter)->diffuse,(*iter)->specular, (*iter)->direction };

			std::string variablesLocation = std::string(cname).append(".dirLightProperties");
			SetUniformFloat3v(variablesLocation, variablesVec3, 5);

			std::string cshadow = std::string(cname).append(".castShadow");
			if ((*iter)->light->castShadows)
			{
				SetUniform1i(cshadow, 1);
				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, (*iter)->depthMap);
				std::string cdepthmap = std::string(cname).append(".depthMap");
				SetUniform1i(cdepthmap, 4);

				(*iter)->light->BindForReading();
				int bakedDepthMap[3] = { 5,6,7 };
				std::string cdepthmapbaked = std::string(cname).append(".bakeShadows");
				SetUniformIntv(cdepthmapbaked, bakedDepthMap, 3);

				char clightspaceM[128];
				sprintf_s(clightspaceM, clightSpaceMatrix.c_str(), i);
				SetUniformMat4f(clightspaceM, (*iter)->projMat * (*iter)->viewMat);

				std::string clightPos = std::string(cname).append(".lightPos");
				SetUniformFloat3(clightPos, (*iter)->fake_position);

				std::string clightPosBaked = std::string(cname).append(".lightPosBaked");
				SetUniformFloat3v(clightPosBaked, (*iter)->fake_position_baked, 3);

				for (int it = 0; it < 3; ++it)
				{
					char clightspaceMB[128];
					sprintf_s(clightspaceMB, clightSpaceMatrixBaked.c_str(), i * 3 + it);
					SetUniformMat4f(clightspaceMB, (*iter)->light->projMatrix * (*iter)->light->viewMatrix[it]);
				}
			}
			else
				SetUniform1i(cshadow, 0);

			++i;
		}
	}
}

void ResourceShader::SetPointLights(const std::string& name, const std::list<PointLightProperties*>& pointLights)
{
	OPTICK_EVENT();

	int i = 0;
	std::string tmp_name(name.c_str());
	tmp_name.append("[%i]");
	for (std::list<PointLightProperties*>::const_iterator iter = pointLights.begin(); iter != pointLights.end(); iter++)
	{
		if ((*iter)->isEnabled())
		{
			char cname[128];
			sprintf_s(cname, tmp_name.c_str(), i);

			// All uniforms 

			std::string cintensity = std::string(cname).append(".intensity");
			SetUniform1f(cintensity, (*iter)->intensity);

			std::string cconstant = std::string(cname).append(".constant");
			SetUniform1f(cconstant, (*iter)->constant);

			std::string clinear = std::string(cname).append(".linear");
			SetUniform1f(clinear, (*iter)->linear);

			std::string cquadratic = std::string(cname).append(".quadratic");
			SetUniform1f(cquadratic, (*iter)->quadratic);

			// Variables Array 
			float3 variablesVec3[4] = { (*iter)->position,(*iter)->ambient, (*iter)->diffuse,(*iter)->specular };

			std::string variablesLocation = std::string(cname).append(".pointLightProperties");
			SetUniformFloat3v(variablesLocation, variablesVec3, 4);

			++i;
		}
	}
}

void ResourceShader::SetSpotLights(const std::string& name, const std::list<SpotLightProperties*>& spotLights)
{
	OPTICK_EVENT();

	int i = 0;
	std::string tmp_name(name.c_str());
	tmp_name.append("[%i]");
	for (std::list<SpotLightProperties*>::const_iterator iter = spotLights.begin(); iter != spotLights.end(); iter++)
	{
		if ((*iter)->isEnabled())
		{
			char cname[128];
			sprintf_s(cname, tmp_name.c_str(), i);

			// All uniforms

			std::string cintensity = std::string(cname).append(".intensity");
			SetUniform1f(cintensity, (*iter)->intensity);

			std::string cconstant = std::string(cname).append(".constant");
			SetUniform1f(cconstant, (*iter)->constant);

			std::string clinear = std::string(cname).append(".linear");
			SetUniform1f(clinear, (*iter)->linear);

			std::string cquadratic = std::string(cname).append(".quadratic");
			SetUniform1f(cquadratic, (*iter)->quadratic);

			std::string ccutoff = std::string(cname).append(".cut_off");
			SetUniform1f(ccutoff, cos(math::DegToRad((*iter)->cut_off)));

			std::string coutercutoff = std::string(cname).append(".outer_cut_off");
			SetUniform1f(coutercutoff, cos(math::DegToRad((*iter)->outer_cut_off)));

			// Variables Array 

			float3 variablesVec3[5] = { (*iter)->position,(*iter)->ambient, (*iter)->diffuse,(*iter)->specular, (*iter)->direction };

			std::string cposition = std::string(cname).append(".spotLightProperties");
			SetUniformFloat3v(cposition, variablesVec3, 5);

			++i;
		}
	}
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
	//assert(vertex_s == 0, "Vertex Failed To Compile");
	uint fragment_s = CompileShader(GL_FRAGMENT_SHADER, fragment_shader);
	//assert(fragment_s == 0, "Fragment Failed To Compile");
	//OriiOri
	glAttachShader(program, vertex_s);
	glAttachShader(program, fragment_s);
		
	glLinkProgram(program);
	int status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint max_length = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

		std::vector<GLchar> infoLog(max_length);
		glGetProgramInfoLog(program, max_length, &max_length, &infoLog[0]);

		std::string str(infoLog.begin(), infoLog.end());

		LOG_ENGINE("%s", str.c_str());

		LOG_ENGINE("Shader was not linked successfully...\n");
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

	int location = glGetUniformLocation(shader_id, name.c_str());
	if (location == -1)
	{
		LOG_ENGINE("WARNING: Uniform %s doesn't exist...\n", name.c_str());
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

void ResourceShader::CreateDepthMap(DirLightProperties* light)
{

}

void ResourceShader::DrawShadows()
{
	// 1. first render to depth map
	//int i = 0;
	//std::string tmp_name("dir_light");
	//tmp_name.append("[%i]");

	//for (std::list<DirLightProperties*>::const_iterator iter = App->objects->directional_light_properites.begin(); iter != App->objects->directional_light_properites.end(); iter++)
	//{
	//	


	//}
}
