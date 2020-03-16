#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API TerrainSwitch : public Alien {

public:
	enum Switches
	{
		SAND,
		DIRT,
		NONE,
	};
	TerrainSwitch();
	virtual ~TerrainSwitch();

	void Start();
	void Update();

	virtual void OnTrigger(ComponentCollider* collider);
	virtual void OnTriggerEnter(ComponentCollider* collider);
	virtual void OnTriggerExit(ComponentCollider* collider);

	//std::string zone_name;
	int switch_id = 0; // temporary
};

ALIEN_FACTORY TerrainSwitch* CreateTerrainSwitch() {
	TerrainSwitch* alien = new TerrainSwitch();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_INPUT_INT(alien->switch_id);
	
	return alien;
}

