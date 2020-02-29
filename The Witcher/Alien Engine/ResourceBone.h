#pragma once
#ifndef _RESOURCE_BONE_H_
#define _RESOURCE_BONE_H_

#include "Resource_.h"
#include "MathGeoLib/include/Math/float4x4.h"

class ResourceBone :
	public Resource
{
public:
	ResourceBone();
	~ResourceBone();

	float4x4 matrix;

	uint num_weights = 0;
	uint* vertex_ids = nullptr;
	float* weights = nullptr;

	bool CreateMetaData(const u64& force_id = 0);
	std::string GetTypeString() const;
	bool LoadMemory();
	void FreeMemory();
	bool ReadBaseInfo(const char* meta_file_path);
};

#endif // !_RESOURCE_BONE_H_


