#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API RockSpawner : public Alien {

public:

	RockSpawner();
	virtual ~RockSpawner();
	
	void Start();
	void FindChildren();
	void Update();

	void Disable();

	void OnDrawGizmos() override;

	float3 GetRandomPositionBtw(const float3& min, const float3& max);

	float min_time_btw = 5.f;
	float max_time_btw = 5.f;
	Prefab rock;

	float rocks_rotation = 5.f;
	float rocks_speed = 5.f;

private:
	float timer = 0.f;
	float time_btw = 0.f;

	GameObject* spawn_0 = nullptr;
	GameObject* spawn_1 = nullptr;

	GameObject* spawn_end = nullptr;

	float3 direction = float3::zero();

	bool enabled = true;
};

ALIEN_FACTORY RockSpawner* CreateRockSpawner() {
	RockSpawner* alien = new RockSpawner();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_PREFAB(alien->rock);

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->min_time_btw);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->max_time_btw);

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->rocks_rotation);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->rocks_speed);

	return alien;
} 
