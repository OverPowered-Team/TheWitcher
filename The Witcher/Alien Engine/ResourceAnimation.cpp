#include "Application.h"
#include "ModuleImporter.h"
#include "Resource_.h"
#include "ResourceAnimation.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "mmgr/mmgr.h"

#include "Optick/include/optick.h"

ResourceAnimation::ResourceAnimation() : Resource()
{
	type = ResourceType::RESOURCE_ANIMATION;
}

ResourceAnimation::~ResourceAnimation()
{
	FreeMemory();
}


std::string ResourceAnimation::GetTypeString() const
{
	return "Animation";
}

float ResourceAnimation::GetDuration() const
{
	return ((float)(end_tick-start_tick) / (float)ticks_per_second);
}

uint ResourceAnimation::GetChannelIndex(std::string name)
{
	uint i;
	for (i = 0; i < num_channels; i++) {
		if (channels[i].name == name) {
			return i;
		}
	}

	return num_channels;
}


bool ResourceAnimation::LoadMemory()
{
	OPTICK_EVENT();

	char* buffer;
	uint size = App->file_system->Load(meta_data_path.data(), &buffer);

	if (size > 0) {
		char* cursor = buffer;

		//Advance cursor to unloaded memory
		uint bytes = sizeof(uint);
		uint name_size;
		memcpy(&name_size, cursor, bytes);
		cursor += bytes;
		bytes = name_size;
		cursor += bytes;
		bytes = sizeof(uint);
		cursor += bytes * 5;
		bytes = sizeof(bool);
		cursor += bytes;

		channels = new ResourceAnimation::Channel[num_channels];
		// Load channels
		for (uint i = 0; i < num_channels; i++)
		{
			//Load channel name size
			bytes = sizeof(uint);
			name_size;
			memcpy(&name_size, cursor, bytes);
			cursor += bytes;

			//Load name
			bytes = name_size;
			channels[i].name.resize(bytes);
			memcpy(&channels[i].name[0], cursor, bytes);
			cursor += bytes;

			//Load num position, scale, rotation keys
			bytes = sizeof(uint);
			memcpy(&channels[i].num_position_keys, cursor, bytes);
			cursor += bytes;
			memcpy(&channels[i].num_scale_keys, cursor, bytes);
			cursor += bytes;
			memcpy(&channels[i].num_rotation_keys, cursor, bytes);
			cursor += bytes;

			//Load position keys
			bytes = sizeof(KeyAnimation<float3>) * channels[i].num_position_keys;
			channels[i].position_keys = new KeyAnimation<float3>[channels[i].num_position_keys];
			memcpy(channels[i].position_keys, cursor, bytes);
			cursor += bytes;

			//Load scale keys
			bytes = sizeof(KeyAnimation<float3>) * channels[i].num_scale_keys;
			channels[i].scale_keys = new KeyAnimation<float3>[channels[i].num_scale_keys];
			memcpy(channels[i].scale_keys, cursor, bytes);
			cursor += bytes;

			//Load rotation keys
			bytes = sizeof(KeyAnimation<Quat>) * channels[i].num_rotation_keys;
			channels[i].rotation_keys = new KeyAnimation<Quat>[channels[i].num_rotation_keys];
			memcpy(channels[i].rotation_keys, cursor, bytes);
			cursor += bytes;
		}

		RELEASE_ARRAY(buffer);
		return true;
	}
	else
	{
		return false;
	}

	return false;
}

void ResourceAnimation::FreeMemory()
{
	if (channels == nullptr)
		return;

	LOG_ENGINE("Unloading Animation %s", name.c_str());

	for (int i = 0; i < num_channels; i++)
	{
		delete[] channels[i].position_keys;
		delete[] channels[i].scale_keys;
		delete[] channels[i].rotation_keys;
	}

	delete[] channels;
	channels = nullptr;
}

bool ResourceAnimation::ReadBaseInfo(const char* meta_file_path)
{
	OPTICK_EVENT();

	meta_data_path = std::string(meta_file_path);
	ID = std::stoull(App->file_system->GetBaseFileName(meta_file_path));

	char* buffer;
	uint size = App->file_system->Load(meta_data_path.data(), &buffer);

	if (size > 0) {
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

		//Load channel nums, duration and ticks per second
		bytes = sizeof(uint);
		memcpy(&start_tick, cursor, bytes);
		cursor += bytes;
		memcpy(&end_tick, cursor, bytes);
		cursor += bytes;
		memcpy(&ticks_per_second, cursor, bytes);
		cursor += bytes;
		memcpy(&max_tick, cursor, bytes);
		cursor += bytes;
		memcpy(&num_channels, cursor, bytes);
		cursor += bytes;

		bytes = sizeof(bool);
		memcpy(&loops, cursor, bytes);

		App->resources->AddResource(this);
		RELEASE_ARRAY(buffer);
		return true;
	}
	else 
		return false;
}

void ResourceAnimation::Copy(ResourceAnimation* anim)
{
	OPTICK_EVENT();

	ticks_per_second = anim->ticks_per_second;
	num_channels = anim->num_channels;
	channels = new ResourceAnimation::Channel[num_channels];

	//copy channels
	uint size = 0;
	for (uint i = 0; i < anim->num_channels; i++)
	{
		channels[i].name = anim->channels[i].name;
		channels[i].num_position_keys = anim->channels[i].num_position_keys;
		channels[i].num_scale_keys = anim->channels[i].num_scale_keys;
		channels[i].num_rotation_keys = anim->channels[i].num_rotation_keys;

		uint bytes = sizeof(KeyAnimation<float3>) * channels[i].num_position_keys;
		channels[i].position_keys = new KeyAnimation<float3>[channels[i].num_position_keys];
		memcpy(channels[i].position_keys, anim->channels[i].position_keys, bytes);

		bytes = sizeof(KeyAnimation<float3>) * channels[i].num_scale_keys;
		channels[i].scale_keys = new KeyAnimation<float3>[channels[i].num_scale_keys];
		memcpy(channels[i].scale_keys, anim->channels[i].scale_keys, bytes);

		bytes = sizeof(KeyAnimation<Quat>) * channels[i].num_rotation_keys;
		channels[i].rotation_keys = new KeyAnimation<Quat>[channels[i].num_rotation_keys];
		memcpy(channels[i].rotation_keys, anim->channels[i].rotation_keys, bytes);
	}
}

bool ResourceAnimation::CreateMetaData(const u64& force_id)
{
	OPTICK_EVENT();

	if (force_id == 0) {
		ID = App->resources->GetRandomID();
	}
	else {
		ID = force_id;
	}
	
	meta_data_path = std::string(LIBRARY_ANIMATIONS_FOLDER + std::to_string(ID) + ".alienAnimation");

	uint size = sizeof(uint) + name.size() + sizeof(uint) * 5 + sizeof(bool);
	for (uint i = 0; i < num_channels; i++)
	{
		size += sizeof(uint) +  channels[i].name.size() + sizeof(uint) * 3 +
			sizeof(KeyAnimation<float3>) *  channels[i].num_position_keys + sizeof(KeyAnimation<float3>) *  channels[i].num_scale_keys + sizeof(KeyAnimation<Quat>) *  channels[i].num_rotation_keys;
	}

	// Allocate
	char* data = new char[size];
	char* cursor = data;

	// Store name size and name
	uint bytes = sizeof(uint);
	uint name_size =  name.size();
	memcpy(cursor, &name_size, bytes);
	cursor += bytes;
	bytes =  name.size();
	memcpy(cursor,  name.c_str(), bytes);
	cursor += bytes;

	// Store duration, ticks and num channels
	bytes = sizeof(uint);
	memcpy(cursor, & start_tick, bytes);
	cursor += bytes;
	memcpy(cursor, & end_tick, bytes);
	cursor += bytes;
	memcpy(cursor, & ticks_per_second, bytes);
	cursor += bytes;
	memcpy(cursor, & max_tick, bytes);
	cursor += bytes;
	memcpy(cursor, & num_channels, bytes);
	cursor += bytes;

	bytes = sizeof(bool);
	memcpy(cursor, & loops, bytes);
	cursor += bytes;

	// Store channels
	for (uint i = 0; i <  num_channels; i++)
	{
		//Store channel name size and name
		bytes = sizeof(uint);
		name_size =  channels[i].name.size();
		memcpy(cursor, &name_size, bytes);
		cursor += bytes;
		bytes =  channels[i].name.size();
		memcpy(cursor,  channels[i].name.c_str(), bytes);
		cursor += bytes;

		//Store num position, scale, rotation keys
		bytes = sizeof(uint);
		memcpy(cursor, & channels[i].num_position_keys, bytes);
		cursor += bytes;
		memcpy(cursor, & channels[i].num_scale_keys, bytes);
		cursor += bytes;
		memcpy(cursor, & channels[i].num_rotation_keys, bytes);
		cursor += bytes;

		//Store position keys
		bytes = sizeof(KeyAnimation<float3>) *  channels[i].num_position_keys;
		memcpy(cursor,  channels[i].position_keys, bytes);
		cursor += bytes;

		//Store scale keys
		bytes = sizeof(KeyAnimation<float3>) *  channels[i].num_scale_keys;
		memcpy(cursor,  channels[i].scale_keys, bytes);
		cursor += bytes;

		//Store rotation keys
		bytes = sizeof(KeyAnimation<Quat>) *  channels[i].num_rotation_keys;
		memcpy(cursor,  channels[i].rotation_keys, bytes);
		cursor += bytes;
	}
	App->file_system->Save(meta_data_path.data(), data, size);
	RELEASE_ARRAY(data);
	return true;
}

bool ResourceAnimation::DeleteMetaData()
{
	return true;
}
