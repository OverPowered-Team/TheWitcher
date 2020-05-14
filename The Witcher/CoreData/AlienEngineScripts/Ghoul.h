#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Enemy.h"

class MusicController;

class Ghoul : public Enemy {
public:
	enum(GhoulState,
		NONE = -1,
		AWAKE,
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

	enum(AwakeBehaviour, 
		DEFAULT,
		SLEEP,
		HIDE,
		WANDER,
		FOLLOW_CURVE)

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
	
	float GetDamaged(float dmg, PlayerController* player, float3 knock_back = float3::zero());

	void OnTriggerEnter(ComponentCollider* collider) override;

	void OnAnimationEnd(const char* name) override;

public:
	GhoulState state = GhoulState::AWAKE;
	GhoulType ghoul_type = GhoulType::NONE;
	AwakeBehaviour awake_behaviour = AwakeBehaviour::FOLLOW_CURVE;
	MusicController* m_controller = nullptr;
	GameObject* awake_curve = nullptr;
};


