#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Enemy.h"

class MusicController;

class ALIEN_ENGINE_API Ghoul : public Enemy {
public:
	enum(GhoulState,
		NONE = -1,
		IDLE,
		MOVE,
		ATTACK,
		JUMP,
		HIT,
		DYING,
		DEAD);

	Ghoul();
	virtual ~Ghoul();

	void SetStats(const char* json) override;
	
	void StartEnemy() override;
	void UpdateEnemy() override;
	void CleanUpEnemy() override;

	void Action() override;
	void CheckDistance() override;
	void JumpImpulse();

	void OnAnimationEnd(const char* name) override;

public:
	GhoulState state = GhoulState::NONE;
};

ALIEN_FACTORY Ghoul* CreateGhoul() {
	Ghoul* ghoul = new Ghoul();
	// To show in inspector here
	SHOW_VOID_FUNCTION(Ghoul::ActivateCollider, ghoul);
	SHOW_VOID_FUNCTION(Ghoul::DeactivateCollider, ghoul);

	return ghoul;
} 
