#include "Application.h"
#include "ModuleImporter.h"

#include "ResourceBone.h"
#include "mmgr/mmgr.h"


ResourceBone::ResourceBone() : Resource()
{
	type = ResourceType::RESOURCE_BONE;
}


ResourceBone::~ResourceBone()
{
	FreeMemory();
}

bool ResourceBone::CreateMetaData(const u64& force_id)
{
	if (force_id == 0) {
		ID = App->resources->GetRandomID();
	}
	else {
		ID = force_id;
	}

	meta_data_path = std::string(LIBRARY_BONES_FOLDER + std::to_string(ID) + ".alienBone");

	uint size = sizeof(uint) + name.size() + sizeof(float4x4) + sizeof(uint) + sizeof(float) * num_weights + sizeof(uint) * num_weights;
	// Allocate
	char* data = new char[size];
	char* cursor = data;

	// Store name size and name
	uint bytes = sizeof(uint);
	uint name_size = name.size();
	memcpy(cursor, &name_size, bytes);
	cursor += bytes;
	bytes = name.size();
	memcpy(cursor, name.c_str(), bytes);
	cursor += bytes;

	//Store matrix
	bytes = sizeof(float4x4);
	memcpy(cursor, &matrix, bytes);
	cursor += bytes;

	//Store num weights
	bytes = sizeof(uint);
	memcpy(cursor, &num_weights, bytes);
	cursor += bytes;

	//Store weights
	bytes = sizeof(float) * num_weights;
	memcpy(cursor, weights, bytes);
	cursor += bytes;

	//Store vertex ids
	bytes = sizeof(uint) * num_weights;
	memcpy(cursor, vertex_ids, bytes);
	cursor += bytes;

	App->file_system->Save(meta_data_path.data(), data, size);
	RELEASE_ARRAY(data);

	return true;
}

std::string ResourceBone::GetTypeString() const
{
	return "Bone";
}

bool ResourceBone::LoadMemory()
{
	char* buffer;
	uint size = App->file_system->Load(meta_data_path.data(), &buffer);
	char* cursor = buffer;

	//Load name size
	uint bytes = sizeof(uint);
	uint name_size;
	memcpy(&name_size, cursor, bytes);
	cursor += bytes;

	//Load name
	bytes = name_size;
	name.resize(bytes);
	memcpy(&name[0], cursor, bytes);
	cursor += bytes;

	//Load matrix
	bytes = sizeof(float4x4);
	memcpy(&matrix, cursor, bytes);
	cursor += bytes;

	//Load weights num
	bytes = sizeof(uint);
	memcpy(&num_weights, cursor, bytes);
	cursor += bytes;

	weights = new float[num_weights];
	vertex_ids = new uint[num_weights];

	//Load weights
	bytes = sizeof(float) * num_weights;
	memcpy(weights, cursor, bytes);
	cursor += bytes;

	//Load vertex ids
	bytes = sizeof(uint) * num_weights;
	memcpy(vertex_ids, cursor, bytes);
	cursor += bytes;

	RELEASE_ARRAY(buffer);
	return true;
}

void ResourceBone::FreeMemory()
{
	if (vertex_ids)
	{
		delete[] vertex_ids;
		vertex_ids = nullptr;
	}
	if (weights)
	{
		delete[] weights;
		weights = nullptr;
	}
		
}

bool ResourceBone::ReadBaseInfo(const char* meta_file_path)
{
	meta_data_path = std::string(meta_file_path);
	ID = std::stoull(App->file_system->GetBaseFileName(meta_file_path));

	char* buffer;
	uint size = App->file_system->Load(meta_data_path.data(), &buffer);
	char* cursor = buffer;

	//Load name size
	uint bytes = sizeof(uint);
	uint name_size;
	memcpy(&name_size, cursor, bytes);
	cursor += bytes;

	//Load name
	bytes = name_size;
	name.resize(bytes);
	memcpy(&name[0], cursor, bytes);
	cursor += bytes;

	App->resources->AddResource(this);

	RELEASE_ARRAY(buffer);
	return true;
}
