#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API RockFall : public Alien {

public:

	RockFall();
	virtual ~RockFall();
	
	void Start();
	void Update();

public:
	Prefab rock;
	float minSpawnTime = 3.0F;
	float maxSpawnTime = 5.0F;
	float force = 20.0F;
	float rangeSpawn = 10.0F;

private:
	float currentTime = 0.0F;
	float spawnTime = 0.0F;

};

ALIEN_FACTORY RockFall* CreateRockFall() {
	RockFall* alien = new RockFall();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_PREFAB(alien->rock);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->minSpawnTime);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->maxSpawnTime);

	return alien;
} 
