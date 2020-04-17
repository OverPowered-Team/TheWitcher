#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API TeleportLevel : public Alien {

public:

	TeleportLevel();
	virtual ~TeleportLevel();
	
	void Start();
	void Update();

public:
	Prefab teleportPrefab;
private:
	std::vector<float3> teleport_pos;
	std::vector<Quat> teleport_rot;
	float4x4 last_matrix;
	int current_pos = 0;
};

ALIEN_FACTORY TeleportLevel* CreateTeleportLevel() {
	TeleportLevel* alien = new TeleportLevel();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_PREFAB(alien->teleportPrefab);
	return alien;
} 
