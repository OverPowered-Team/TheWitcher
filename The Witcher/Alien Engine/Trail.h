#pragma once

#include "Globals.h"
#include "Timer.h"
#include <list>
#include "GameObject.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/quat.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include <vector>
#include "ResourceMaterial.h"
#define MAX_TRAIL_NODE 500

struct TrailNode
{
	math::float3 originHigh = float3::zero();
	math::float3 originLow = float3::zero();

	Uint32 createTime = 0u;
	bool to_delete = false;
	float current_lifeTime = 0;

	TrailNode(math::float3 high, math::float3 low)
	{
		originHigh = high;
		originLow = low;

		createTime = SDL_GetTicks();
	}
};

enum class TrailVector
{
	X,
	Y,
	Z
};


class __declspec(dllexport) Trail
{
	friend class ComponentTrail;
public:

	Trail();
	Trail(ComponentTrail* trail_comp, GameObject* owner);
	~Trail();

private:

	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool Draw();

	void RearrangeVertex(Trail* trail, std::list<TrailNode*>::iterator& curr, std::list<TrailNode*>::iterator& next, float& currUV, float& nextUV, math::float3& originHigh, math::float3& originLow, math::float3& destinationHigh, math::float3& destinationLow);
	void GetOriginAndDest(Trail* trail, float& origin, std::list<TrailNode*>::iterator& curr, float& dest, std::list<TrailNode*>::iterator& next);
	void SetMaterial(ResourceMaterial* mat);
	void RemoveMaterial();
	void UpdateUniforms(ResourceMaterial* resource_material, float4x4 globalMatrix, float currUV, float nextUV, float3 originHigh, float3 originLow, float3 destHigh, float3 destLow);

public:
	void Start();
	void Stop();

	bool isPlaying() const;


	void SetVector(TrailVector vec);
	inline TrailVector GetVector() { return vector; }

	void SetTime(int time);
	inline int GetTime() { return time; }

	void SetLifeTime(int lifeTime);
	inline int GetLifeTime() { return lifeTime; }

	void SetMinDistance(float minDistance);
	inline float GetMinDistance() { return minDistance; }

	void SetSpawnSize(math::float3 size);
	inline float3 GetSpawnSize() { return originalSpawnBox.Size(); }

	void SetColor(math::float4 color);
	inline math::float4 GetColor() { return color; }

	
public:
	std::list<TrailNode*> trailVertex;
	
	Timer timer;
	TrailVector vector = TrailVector::X;
	bool emitting = true;
	bool customSpawn = false;


	float minDistance = 0.05f;
	float4 color = float4::one;
	float lifeTime = 0.05;
	float time = 5;
	float current_time = 0;
	bool orient = false;

	int high = 5;
	int low = 4;

	math::OBB spawnBox;
	math::OBB _spawnBox;
	math::OBB originalSpawnBox;


	GameObject* owner = nullptr;
	ComponentTrail* trail_comp = nullptr;

	ResourceMaterial* material = nullptr;
	ResourceMaterial* default_material = nullptr;

	// Vertex Array Object
	uint vao = 0u;

	// Buffers id
	uint id_index = 0;
	uint id_vertex = 0;
	uint id_uv = 0;

};