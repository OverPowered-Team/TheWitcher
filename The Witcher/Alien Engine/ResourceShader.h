#ifndef __RESOURCE_SHADER_H__
#define __RESOURCE_SHADER_H__

#include <unordered_map>

#include "MathGeoLib/include/MathGeoLib.h"

#include "Resource_.h"

struct SHADER_PROGRAM_SOURCE
{
	std::string vertex_source;
	std::string fragment_source;
};

enum class SHADER_TYPE
{
	UNKNOWN = -1,
	VERTEX,
	FRAGMENT
};

enum class SHADER_TEMPLATE
{
	NO_TEMPLATE = -1,
	DIFUSSE,
	WAVE,
	BASIC_LIGHTING
};

struct UniformData
{
	struct StandardShaderProperties 
	{
		float3 diffuse_color = float3::one();
	} standardShaderProperties;

	struct WaveShaderProperties
	{
		float mult_time = 1.0f;
		float amplitude = 0.75f;
	} waveShaderProperties;

	struct BasicLightingShaderProperties {
		float3 object_color = float3(1.f, 1.f, 1.f);

		// Lighting
		float ambient_strength = 0.1f;
		float specular_strength = 0.5f;
		float3 lightPosition = float3(5.f, 5.f, 5.f);
		float3 lightColor = float3(1.f, 1.f, 1.f);
	} basicLightingShaderProperties;

	uint shader_id = 0;
	uint textures_id = 0;
	SHADER_TEMPLATE type = SHADER_TEMPLATE::DIFUSSE;
};

class ResourceShader : public Resource
{
	friend class ComponentMaterial;

public:
	ResourceShader()
	{
		type = ResourceType::RESOURCE_SHADER; 
	}
	ResourceShader(const char* path, const u64& id);
	ResourceShader(const char* path);
	virtual ~ResourceShader();

	bool LoadMemory();
	void FreeMemory();

	bool CreateMetaData(const u64& force_id = 0);
	bool DeleteMetaData();

	bool ReadBaseInfo(const char* assets_file_path);
	void ReadLibrary(const char* meta_data);

public:
	uint ParseAndCreateShader();
	bool ChangeTemplate();
	void HierarchyUniforms();


	void UpdateUniforms();
	void Bind() const;
	void Unbind() const;
	
	void SetUniform1i(const std::string& name, const int& value);
	void SetUniform1f(const std::string& name, const float& value);

	void SetUniformFloat3(const std::string& name, const float3& vec3);

	void SetUniform4f(const std::string& name, const float& v0, const float& v1, const float& v2, const float& v3);
	void SetUniform4f(const std::string& name, const float4& vec);

	void SetUniformMat4f(const std::string& name, const math::float4x4& matrix);

	void CreateShaderDoc(const int& type, const char* name);

private:
	SHADER_PROGRAM_SOURCE ParseShader(const std::string& path);
	uint CreateShader(const std::string& vertex_shader, const std::string& fragment_shader);
	
	uint CompileShader(const uint& shader_type, const std::string& shader_source);

	int GetUniformLocation(const std::string& name);

	// TODO: Create uniform cache for optimization and faster search.

private:
	UniformData uniform_data;
	std::unordered_map<std::string, int> uniform_location_cache;
};

#endif /* __RESOURCE_SHADER_H__ */