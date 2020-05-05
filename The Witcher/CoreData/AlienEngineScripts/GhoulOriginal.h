#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Ghoul.h"

class ALIEN_ENGINE_API GhoulOriginal : public Ghoul {

public:

	GhoulOriginal();
	virtual ~GhoulOriginal();

	void UpdateEnemy() override;

};

ALIEN_FACTORY GhoulOriginal* CreateGhoulOriginal() {
	GhoulOriginal* ghoul = new GhoulOriginal();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_ENUM(Ghoul::GhoulType, ghoul->ghoul_type);
	SHOW_IN_INSPECTOR_AS_ENUM(Ghoul::GhoulState, ghoul->state);
	SHOW_VOID_FUNCTION(Ghoul::ActivateCollider, ghoul);
	SHOW_VOID_FUNCTION(Ghoul::DeactivateCollider, ghoul);
	SHOW_VOID_FUNCTION(Enemy::SpawnAttackParticle, ghoul);

	return ghoul;
}