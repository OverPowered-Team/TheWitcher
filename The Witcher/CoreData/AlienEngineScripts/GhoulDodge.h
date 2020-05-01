#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Ghoul.h"

class ALIEN_ENGINE_API GhoulDodge : public Ghoul {

public:

	GhoulDodge();
	virtual ~GhoulDodge();
	
	void UpdateEnemy() override;

};

ALIEN_FACTORY GhoulDodge* CreateGhoulDodge() {
	GhoulDodge* ghoul = new GhoulDodge();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_ENUM(Ghoul::GhoulType, ghoul->ghoul_type);
	SHOW_IN_INSPECTOR_AS_ENUM(Ghoul::GhoulState, ghoul->state);
	SHOW_VOID_FUNCTION(Ghoul::ActivateCollider, ghoul);
	SHOW_VOID_FUNCTION(Ghoul::DeactivateCollider, ghoul);

	return ghoul;
}
