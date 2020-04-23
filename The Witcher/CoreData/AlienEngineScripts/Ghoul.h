#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Enemy.h"

class ALIEN_ENGINE_API Ghoul : public Enemy {

public:

	Ghoul();
	virtual ~Ghoul();

	void SetStats(const char* json) override;
	
	void StartEnemy() override;
	void UpdateEnemy() override;
	void CleanUpEnemy() override;

	void Attack() override;
	void JumpImpulse();

	void OnAnimationEnd(const char* name) override;

public:
	float maxForce = 40.0F;

};

ALIEN_FACTORY Ghoul* CreateGhoul() {
	Ghoul* ghoul = new Ghoul();
	// To show in inspector here	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(Enemy::EnemyState, ghoul->state);
	SHOW_VOID_FUNCTION(Ghoul::ActivateCollider, ghoul);
	SHOW_VOID_FUNCTION(Ghoul::DeactivateCollider, ghoul);

	return ghoul;
} 
