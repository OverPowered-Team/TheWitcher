#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Ghoul.h"

class ALIEN_ENGINE_API GhoulOriginal : public Ghoul {

public:

	GhoulOriginal();
	virtual ~GhoulOriginal();

	void UpdateEnemy() override;
	void DoAwake(); 

private: 
	float current_curve_point = 0.f; 

public: 
	float curve_speed = 0.02f; 

};

ALIEN_FACTORY GhoulOriginal* CreateGhoulOriginal() {
	GhoulOriginal* ghoul = new GhoulOriginal();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_ENUM(Ghoul::GhoulType, ghoul->ghoul_type);
	SHOW_IN_INSPECTOR_AS_ENUM(Ghoul::GhoulState, ghoul->state);
	SHOW_IN_INSPECTOR_AS_ENUM(Ghoul::AwakeBehaviour, ghoul->awake_behaviour); 
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(ghoul->curve_speed); 
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(ghoul->increase_hit_animation);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(ghoul->awake_curve); 
	SHOW_VOID_FUNCTION(Ghoul::ActivateCollider, ghoul);
	SHOW_VOID_FUNCTION(Ghoul::DeactivateCollider, ghoul);
	SHOW_VOID_FUNCTION(Enemy::SpawnAttackParticle, ghoul);

	return ghoul;
}