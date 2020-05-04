#pragma once

#include "Resource_.h"
#include "Color.h"
#include "MathGeoLib/include/Math/float4.h"
#include <vector>

#define NO_TEXTURE_ID 0

#define DEFAULT_SMOOTHNESS 32.f
#define DEFAULT_METALNESS 0.5f

enum class TextureType {
	NONE = -1,
	DIFFUSE = 0,
	SPECULAR,
	NORMALS,
	HEIGHT,
	AMBIENT,
	EMISSIVE,
	SHININESS,
	OPACITY,
	DISPLACEMENT,
	LIGHTMAP,
	REFLECTION,
	DIFFUSE_ROUGHNESS,
	AMBIENT_OCCLUSION,
	METALNESS,

	MAX
};

struct ShaderInputs
{
	struct StandardShaderProperties
	{
		float4 diffuse_color = float4::one;
		float smoothness = DEFAULT_SMOOTHNESS;
		float metalness = DEFAULT_METALNESS;
	} standardShaderProperties;

	struct WaveShaderProperties
	{
		float mult_time = 1.0f;
		float amplitude = 0.75f;
	} waveShaderProperties;

	struct IluminatedShaderProperties {
		float3 object_color = float3(1.f, 1.f, 1.f);
	} iluminatedShaderProperties;

	struct ParticleShaderProperties {
		float4 color = float4(1.f, 0.f, 0.8f, 1.0f);
		//float3 start_color = float3(1.f, 0.f, 0.8f);
		//float3 end_color = float3(1.f, 1.f, 1.f);
	} particleShaderProperties;


	struct ShieldShaderProperties {
		float3 color = float3(1.f, 1.f, 1.f);
		float3 hit_position = float3(0.9757f, -0.2074f, -0.0703f);
	} shieldShaderProperties; 

	struct ShieldFresnelShaderProperties {
		float4 color = float4(1.f, 1.f, 1.f,1.f);
		float shieldStrength = 1.0f;
		float shieldCooldown = 1.0f;
		float fresnel_exponent = 0.75f;
		//static const int32_t MAX_SHIELD_HITS = 5;
		//float3 hitPos[MAX_SHIELD_HITS];
		//float radii[MAX_SHIELD_HITS];
		//int numHits = 1;

	} shieldFresnelShaderProperties;

};

class ResourceShader; 
class ResourceTexture; 
class JSONfilepack;

class ResourceMaterial : public Resource {

public:

	ResourceMaterial();
	virtual ~ResourceMaterial();

	bool LoadMemory() override;
	void FreeMemory() override; 

	void OnSelected() override; 
	void OnDeselected() override;

	void SaveResource() override; 

	// meta data
	bool CreateMetaData(const u64& force_id = 0);
	bool ReadBaseInfo(const char* assets_file_path);
	void ReadLibrary(const char* meta_data);
	bool DeleteMetaData();

	void SaveMaterialValues(JSONfilepack* file);
	void ReadMaterialValues(JSONfilepack* file);

	// Functionality
	void ApplyMaterial();
	void UnbindMaterial();

	void ApplyPreRenderShadows();

	void SetTexture(ResourceTexture* texture, TextureType texType = TextureType::DIFFUSE);
	const ResourceTexture* GetTexture(TextureType texType = TextureType::DIFFUSE) const;
	ResourceTexture* GetTexture(TextureType texType = TextureType::DIFFUSE);
	void RemoveTexture(TextureType texType = TextureType::DIFFUSE);
	
	bool HasTexture(TextureType texType = TextureType::DIFFUSE) const;

	void SetShader(ResourceShader* newShader);
	// ----- ImGui ----- 
	void DisplayMaterialOnInspector();
	void MaterialHeader();
	void ShaderSelectionHeader();
	void ShaderInputsSegment();
	void InputTexture(TextureType texType);
	void TexturesSegment();

	void TextureBrowser(TextureType texType);
	
	// ------------------ 

public:

	float4 color = float4::one;

	bool textureActivated = true;
	bool recive_shadow = false;
	std::pair<u64, ResourceTexture*> textures[(uint)TextureType::MAX];
	TextureType selectedType = TextureType::NONE;
	ShaderInputs shaderInputs;
	u64 used_shader_ID = 0;
	ResourceShader* simple_depth_shader = nullptr; 
	ResourceShader* used_shader = nullptr;
	const char* selectedShader = nullptr;
	ResourceTexture* selected_texture = nullptr;
	bool change_texture_menu = false;
};
