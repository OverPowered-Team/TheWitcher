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
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(ghoul->gravity);
	SHOW_IN_INSPECTOR_AS_PREFAB(ghoul->head_prefab);
	SHOW_IN_INSPECTOR_AS_PREFAB(ghoul->life_orb);

	/*SHOW_SPACING();
	SHOW_SEPARATOR();
	SHOW_TEXT("What does the enemy do before aggro-ing the players?");
	SHOW_IN_INSPECTOR_AS_ENUM(Ghoul::AwakeBehaviour, ghoul->awake_behaviour); 

	SHOW_SPACING();
	SHOW_SEPARATOR();
	SHOW_TEXT("If follow curve");
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(ghoul->awake_curve);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(ghoul->curve_speed); 
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(ghoul->patrol);

	SHOW_SPACING();
	SHOW_SEPARATOR();
	SHOW_TEXT("If wander");
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(ghoul->wander_radius);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(ghoul->wander_speed);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(ghoul->wander_rest_time);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(ghoul->wander_precision);*/

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
	SHOW_VOID_FUNCTION(Enemy::PlayAttackSFX, ghoul);

	return ghoul;
}