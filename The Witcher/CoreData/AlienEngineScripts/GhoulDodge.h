#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Ghoul.h"

class ALIEN_ENGINE_API GhoulDodge : public Ghoul {

public:

	GhoulDodge();
	virtual ~GhoulDodge();
	
	void UpdateEnemy() override;

	void Dodge();

	bool CheckIfDodge();

	void OnAnimationEnd(const char* name) override;

public: 
	int rand_num = -1;

};

ALIEN_FACTORY GhoulDodge* CreateGhoulDodge() {
	GhoulDodge* ghoul = new GhoulDodge();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_ENUM(Ghoul::GhoulType, ghoul->ghoul_type);
	SHOW_IN_INSPECTOR_AS_ENUM(Ghoul::GhoulState, ghoul->state);
	SHOW_IN_INSPECTOR_AS_PREFAB(ghoul->head_prefab);
	SHOW_IN_INSPECTOR_AS_PREFAB(ghoul->life_orb);
	//SHOW_IN_INSPECTOR_AS_ENUM(Ghoul::AwakeBehaviour, ghoul->awake_behaviour);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(ghoul->gravity);
	//SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(ghoul->curve_speed);
	//SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(ghoul->wander_radius);
	//SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(ghoul->wander_speed);
	//SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(ghoul->wander_rest_time);
	//SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(ghoul->wander_precision);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(ghoul->increase_hit_animation);
	//SHOW_IN_INSPECTOR_AS_GAMEOBJECT(ghoul->awake_curve);
	SHOW_VOID_FUNCTION(Ghoul::CanJump, ghoul);
	SHOW_VOID_FUNCTION(Ghoul::CanNotJump, ghoul);
	SHOW_VOID_FUNCTION(Ghoul::ActivateRangeCollider, ghoul);
	SHOW_VOID_FUNCTION(Ghoul::DeactivateRangeCollider, ghoul);
	SHOW_VOID_FUNCTION(Ghoul::ActivateCollider, ghoul);
	SHOW_VOID_FUNCTION(Ghoul::DeactivateCollider, ghoul);
	SHOW_VOID_FUNCTION(Enemy::SpawnAttackParticle, ghoul); 
	SHOW_VOID_FUNCTION(Enemy::CanGetInterrupted, ghoul);
	SHOW_VOID_FUNCTION(Enemy::SpawnHealthOrb, ghoul);
	SHOW_VOID_FUNCTION(Enemy::PlayAttackSFX, ghoul);

	return ghoul;
}
