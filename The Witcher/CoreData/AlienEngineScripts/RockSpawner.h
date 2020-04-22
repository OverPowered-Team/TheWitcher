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

	void OnDrawGizmos() override;

	float3 GetRandomPositionBtw(const float3& min, const float3& max);

	float time_btw = 5.f;
	Prefab rock;

private:
	float timer = 0.f;

	GameObject* spawn_0 = nullptr;
	GameObject* spawn_1 = nullptr;

	GameObject* spawn_end = nullptr;

	float3 direction = float3::zero();
};

ALIEN_FACTORY RockSpawner* CreateRockSpawner() {
	RockSpawner* alien = new RockSpawner();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_PREFAB(alien->rock);

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->time_btw);

	return alien;
} 
