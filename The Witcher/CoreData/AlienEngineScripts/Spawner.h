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

	void Spawn(TO_SPAWN type, float3 position, int head_type);

	// Spawn Elements
	Prefab ghoul_head;
	Prefab nilfgaardian_head;
	Prefab leshen_head;
	Prefab ciri_head;
	Prefab drowned_head;
	Prefab coin;
	Prefab relic;
};

ALIEN_FACTORY Spawner* CreateSpawner() {
	Spawner* alien = new Spawner();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_PREFAB(alien->ghoul_head);
	SHOW_IN_INSPECTOR_AS_PREFAB(alien->nilfgaardian_head);
	SHOW_IN_INSPECTOR_AS_PREFAB(alien->leshen_head);
	SHOW_IN_INSPECTOR_AS_PREFAB(alien->ciri_head);
	SHOW_IN_INSPECTOR_AS_PREFAB(alien->drowned_head);
	SHOW_IN_INSPECTOR_AS_PREFAB(alien->coin);
	SHOW_IN_INSPECTOR_AS_PREFAB(alien->relic);

	return alien;
} 
