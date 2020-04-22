#pragma once
#include "Resource_.h"
class ResourceAudio :
	public Resource
{
public:
	ResourceAudio();
	~ResourceAudio();

	bool CreateMetaData(const u64& force_id);
	bool ReadBaseInfo(const char* assets_file_path);
	void ReadLibrary(const char* meta_data);
};

