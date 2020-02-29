#pragma once
#ifndef _RESOURCE_ANIMATION_H_
#define _RESOURCE_ANIMATION_H_

#include "Resource.h"
#include <map>
template<typename TYPE>
struct KeyAnimation
{
	double time;
	TYPE value;
	KeyAnimation() {};
	KeyAnimation(TYPE value, double time) :value(value), time(time) {};
};

class ResourceAnimation : public Resource
{
public:
	struct Channel {
		std::string name = "";
		uint num_position_keys = 0;
		uint num_scale_keys = 0;
		uint num_rotation_keys = 0;

		KeyAnimation<float3>* position_keys;
		KeyAnimation<float3>* scale_keys;
		KeyAnimation<Quat>* rotation_keys;
	};

public:

	ResourceAnimation();
	~ResourceAnimation();

	std::string GetTypeString() const;
	float GetDuration() const;

	uint GetChannelIndex(std::string name);

	bool LoadMemory();
	void FreeMemory();
	bool ReadBaseInfo(const char* meta_file_path);
	void Copy(ResourceAnimation* anim);

	bool CreateMetaData(const u64& force_id = 0) override;
	bool DeleteMetaData() override;

public:
	bool loops = false;
	uint ticks_per_second = 0;
	uint start_tick = 0;
	uint end_tick = 0;
	uint max_tick = 0;

	uint num_channels = 0;
	Channel* channels  = nullptr;
};


#endif // !_RESOURCE_ANIMATION_H_


