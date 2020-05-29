#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Drowned.h"

class ALIEN_ENGINE_API DrownedRange : public Drowned {

public:

	DrownedRange();
	virtual ~DrownedRange();
	
	void UpdateEnemy() override;

	void ShootSlime();
	void OnAnimationEnd(const char* name) override;

public:
	float current_hide_time = 0.0f;
	float max_hide_time = 3.0f;
	float movement = 0.0f;
	Prefab slime;

	bool set_attack = false;
};

ALIEN_FACTORY DrownedRange* CreateDrownedRange() {
	DrownedRange* drowned = new DrownedRange();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_PREFAB(drowned->slime);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(drowned->max_hide_time);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(drowned->gravity);
	SHOW_IN_INSPECTOR_AS_PREFAB(drowned->head_prefab);
	SHOW_IN_INSPECTOR_AS_ENUM(Drowned::DrownedType, drowned->drowned_type);
	SHOW_IN_INSPECTOR_AS_ENUM(Drowned::DrownedState, drowned->state);
	SHOW_VOID_FUNCTION(DrownedRange::ShootSlime, drowned);
	SHOW_VOID_FUNCTION(Enemy::SpawnAttackParticle, drowned);
	SHOW_VOID_FUNCTION(Enemy::CanGetInterrupted, drowned);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(drowned->increase_hit_animation);

	return drowned;
} 
