#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Enemy.h"
#include "BlockerObstacle.h"

class MusicController;

class Drowned : public Enemy {
public:
	enum(DrownedType,
		NONE = -1,
		RANGE = 0,
		GRAB = 1);

	enum(DrownedState,
		NONE = -1,
		IDLE,
		IDLE_OUT,
		MOVE,
		ATTACK,
		GETOFF,
		HIDE,
		STUNNED,
		HIT,
		DYING,
		DEAD);

	Drowned();
	virtual ~Drowned();
	
	void StartEnemy() override;
	void SetStats(const char* json) override;

	float GetDamaged(float dmg, PlayerController* player, float3 knock_back = float3::zero()) override;
	void SetState(const char* state_str) override;

	void Stun(float time) override;
	bool IsDead() override;
	bool IsDying() override;
	bool IsHit() override;
	bool IsState(const char*) override;
	void PlaySFX(const char* sfx_name);

	void OnAnimationEnd(const char* name) override {};
public:
	DrownedState state = DrownedState::NONE;
	DrownedType drowned_type = DrownedType::NONE;
	MusicController* m_controller = nullptr;
	bool is_hide = true;
};

