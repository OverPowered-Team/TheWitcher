#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Ghoul.h"

class ALIEN_ENGINE_API GhoulMini : public Ghoul {

public:

	GhoulMini();
	virtual ~GhoulMini();
	
	void StartEnemy();
	void UpdateEnemy();

};

ALIEN_FACTORY GhoulMini* CreateGhoulMini() {
	GhoulMini* ghoul = new GhoulMini();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(Ghoul::GhoulType, ghoul->ghoul_type);
	SHOW_IN_INSPECTOR_AS_ENUM(Ghoul::GhoulState, ghoul->state);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(ghoul->gravity);
	SHOW_IN_INSPECTOR_AS_PREFAB(ghoul->head_prefab);
	SHOW_IN_INSPECTOR_AS_PREFAB(ghoul->life_orb);

	SHOW_SPACING();
	SHOW_SEPARATOR();

	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(ghoul->increase_hit_animation);
	SHOW_VOID_FUNCTION(Ghoul::CanJump, ghoul);
	SHOW_VOID_FUNCTION(Ghoul::CanNotJump, ghoul);
	SHOW_VOID_FUNCTION(Ghoul::ActivateCollider, ghoul);
	SHOW_VOID_FUNCTION(Ghoul::DeactivateCollider, ghoul);
	SHOW_VOID_FUNCTION(Ghoul::ActivateRangeCollider, ghoul);
	SHOW_VOID_FUNCTION(Ghoul::DeactivateRangeCollider, ghoul);
	SHOW_VOID_FUNCTION(Enemy::SpawnAttackParticle, ghoul);
	SHOW_VOID_FUNCTION(Enemy::CanGetInterrupted, ghoul);
	SHOW_VOID_FUNCTION(Enemy::SpawnHealthOrb, ghoul);

	return ghoul;
} 
