#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class Scale_Win;

class ALIEN_ENGINE_API Head_Script : public Alien {

public:

	Head_Script();
	virtual ~Head_Script();

	void OnTriggerEnter(ComponentCollider* collider);

	int points = 0;

	Scale_Win* script_scale = nullptr;

	bool first = true;
};

ALIEN_FACTORY Head_Script* CreateHead_Script() {
	Head_Script* alien = new Head_Script();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_INPUT_INT(alien->points);

	return alien;
} 
