#pragma once

#include "Resource_.h"
#include "Color.h"
#include <vector>

/*

enum aiTextureType
{
	
	aiTextureType_NONE = 0,


	aiTextureType_DIFFUSE = 1,


	aiTextureType_SPECULAR = 2,


	aiTextureType_AMBIENT = 3,


	aiTextureType_EMISSIVE = 4,


	aiTextureType_HEIGHT = 5,


	aiTextureType_NORMALS = 6,


	aiTextureType_SHININESS = 7,


	aiTextureType_OPACITY = 8,


	aiTextureType_DISPLACEMENT = 9,


	aiTextureType_LIGHTMAP = 10,

	aiTextureType_REFLECTION = 11,


	aiTextureType_BASE_COLOR = 12,
	aiTextureType_NORMAL_CAMERA = 13,
	aiTextureType_EMISSION_COLOR = 14,
	aiTextureType_METALNESS = 15,
	aiTextureType_DIFFUSE_ROUGHNESS = 16,
	aiTextureType_AMBIENT_OCCLUSION = 17,

};
*/

enum class TextureType {
	NONE = -1,
	DIFFUSE = 0,
	SPECULAR,
	AMBIENT,
	EMISSIVE,
	HEIGHT,
	NORMALS,
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



class ResourceMaterial : public Resource {

public:

	ResourceMaterial();
	virtual ~ResourceMaterial();

	// meta data
	bool CreateMetaData(const u64& force_id = 0);
	bool ReadBaseInfo(const char* assets_file_path);
	void ReadLibrary(const char* meta_data);
	bool DeleteMetaData();

public:
	
	Color color = Color::White();
	u64 texturesID[(uint)TextureType::MAX] = { 0 };

};
