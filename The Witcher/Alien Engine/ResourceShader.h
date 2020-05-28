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
	DEFAULT,
	WAVE,
	ILUMINATED,
	PARTICLE,
	SHIELD,
	SHIELD_FRESNEL,
	SHADOW,
	WATER

};

struct ShaderInputs;
struct DirLightProperties; 
struct PointLightProperties; 
struct SpotLightProperties; 

class ResourceShader : public Resource
{
	friend class ComponentMaterial;
	friend class ComponentText;

public:
	ResourceShader();	
	ResourceShader(const char* path, const u64& id);
	ResourceShader(const char* path);
	virtual ~ResourceShader();

	bool LoadMemory();
	void FreeMemory();

	bool CreateMetaData(const u64& force_id = 0);
	bool DeleteMetaData();

	bool ReadBaseInfo(const char* assets_file_path);
	void ReadLibrary(const char* meta_data);

	void TryToSetShaderType();

public:

	uint ParseAndCreateShader();

	bool ChangeTemplate();
	SHADER_TEMPLATE GetShaderType() const;

	void UpdateUniforms(ShaderInputs inputs);
	void ApplyCurrentShaderGlobalUniforms(ComponentCamera* camera); 
	void ApplyLightsUniforms();
	void Bind() const;
	void Unbind() const;

	
	void SetUniform1i(const std::string& name, const int& value);
	void SetUniform1ui(const std::string& name, const uint& value);
	void SetUniform1f(const std::string& name, const float& value);

	void SetUniform3i(const std::string& name, const int& v0, const int& v1, const int& v2);

	void SetUniformFloat3(const std::string& name, const float3& vec3);

	void SetUniformFloat3v(const std::string& name, const float3* vec, uint count);

	void SetUniformFloatv(const std::string& name, const float* vec, uint count);

	void SetUniformIntv(const std::string& name, const int* vec, uint count);

	void SetUniform4f(const std::string& name, const float& v0, const float& v1, const float& v2, const float& v3);
	void SetUniform4f(const std::string& name, const float4& vec);

	void SetUniformMat4f(const std::string& name, const math::float4x4& matrix);
	void SetUniformMat4f(const std::string& name, const math::float4x4* matrix, uint count);

	void SetDirectionalLights(const std::string& name,  const std::list<DirLightProperties*>& dirLights);
	void SetPointLights(const std::string& name, const std::list<PointLightProperties*>& dirLights);
	void SetSpotLights(const std::string& name, const std::list<SpotLightProperties*>& dirLights);
	void CreateShaderDoc(const int& type, const char* name);

	void CreateDepthMap(DirLightProperties* light);
	void DrawShadows();

private:
	SHADER_PROGRAM_SOURCE ParseShader(const std::string& path);
	uint CreateShader(const std::string& vertex_shader, const std::string& fragment_shader);
	
	uint CompileShader(const uint& shader_type, const std::string& shader_source);

	int GetUniformLocation(const std::string& name);

private:
	uint shader_id;
	SHADER_TEMPLATE shaderType = SHADER_TEMPLATE::DEFAULT;
	std::unordered_map<std::string, int> uniform_location_cache;
public:
	bool has_shadow = false;
	//uint depthMapFBO;

};

#endif /* __RESOURCE_SHADER_H__ */
