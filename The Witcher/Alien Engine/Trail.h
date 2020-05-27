#pragma once

#include "Globals.h"
#include "Timer.h"
#include <list>
#include "GameObject.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/quat.h"
#include "MathGeoLib/include/MathGeoLib.h"

#include <vector>

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
	

public:
	void Start();
	void Stop();
	
public:
	std::list<TrailNode*> trailVertex;
	Timer timer;
	TrailVector vector = TrailVector::X;
	bool create = true;
	bool customSpawn = false;


	float minDistance = 0.05f;
	int lifeTime = 200;

	int high = 5;
	int low = 4;

	math::OBB spawnBox;
	math::OBB _spawnBox;
	math::OBB originalSpawnBox;


	GameObject* owner = nullptr;
	ComponentTrail* trail_comp = nullptr;

	// Vertex Array Object
	uint vao = 0u;

	// Buffers id
	uint id_index = 0;
	uint id_vertex = 0;
	uint id_uv = 0;

};