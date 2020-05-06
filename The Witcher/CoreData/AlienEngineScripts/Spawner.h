#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

enum(TO_SPAWN,
	HEAD,
	COIN,
	RELIC,
	MAX
)

class ALIEN_ENGINE_API Spawner : public Alien {

public:

	Spawner();
	virtual ~Spawner();

	void Spawn(TO_SPAWN type, float3 position);

	// Spawn Elements
	Prefab enemy_heads;
	Prefab coin;
	Prefab relic;
};

ALIEN_FACTORY Spawner* CreateSpawner() {
	Spawner* alien = new Spawner();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_PREFAB(alien->enemy_heads);
	SHOW_IN_INSPECTOR_AS_PREFAB(alien->coin);
	SHOW_IN_INSPECTOR_AS_PREFAB(alien->relic);

	return alien;
} 
