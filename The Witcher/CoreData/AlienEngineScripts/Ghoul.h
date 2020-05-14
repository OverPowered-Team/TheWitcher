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

	void OnDrawGizmosSelected(); 

// Group tactics
	//void OnGroupStrengthChange(float strength_multi) override;

// = = = = = = = = = = = = = = = = = = = = = = AWAKE BEHAVIOURS = = = = = = = = = = = = = = = = = = = = = =  
public:
	float current_curve_point = 0.f;
	float curve_speed = 0.02f;
	float wander_speed = 2.0f; 
	float wander_radius = 0.0f;
	float wander_precision = 0.5f; 
	float wander_rest_time = 1.f;
	float current_wander_time = 0.f; 
	bool wander_rest = true; 
	float3 lastWanderTargetPos; 
// = = = = = = = = = = = = = = = = = = = = = = AWAKE BEHAVIOURS = = = = = = = = = = = = = = = = = = = = = =  

public:
	GhoulState state = GhoulState::AWAKE;
	GhoulType ghoul_type = GhoulType::NONE;
	AwakeBehaviour awake_behaviour = AwakeBehaviour::DEFAULT;
	MusicController* m_controller = nullptr;
	GameObject* awake_curve = nullptr;
};


