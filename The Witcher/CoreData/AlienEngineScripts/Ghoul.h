#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Enemy.h"

class MusicController;

class Ghoul : public Enemy {
public:
	enum(GhoulState,
		NONE = -1,
		IDLE,
		MOVE,
		ATTACK,
		JUMP,
		DODGE,
		STUNNED,
		HIT,
		DYING,
		DEAD);

	enum(GhoulType,
		NONE = -1,
		ORIGINAL,
		DODGE);

	Ghoul();
	virtual ~Ghoul();

	void SetStats(const char* json) override;
	
	void StartEnemy() override;
	void CleanUpEnemy() override;

	void JumpImpulse();

	void Stun(float time) override;
	bool IsDead() override;
	void SetState(const char* state_str) override;

	void Action();

	void CheckDistance();
	
	float GetDamaged(float dmg, PlayerController* player);

	void OnTriggerEnter(ComponentCollider* collider) override;

	void OnAnimationEnd(const char* name) override;

public:
	GhoulState state = GhoulState::NONE;
	GhoulType ghoul_type = GhoulType::NONE;
};


