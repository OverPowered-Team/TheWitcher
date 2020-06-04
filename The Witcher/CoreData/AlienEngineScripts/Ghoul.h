#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Enemy.h"
#include "BlockerObstacle.h"
class MusicController;

class Ghoul : public Enemy {
public:
	enum(GhoulState,
		NONE = -1,
		AWAKE,
		IDLE,
		MOVE,
		ATTACK,
		GUARD,
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
	bool IsDying() override;
	void SetState(const char* state_str) override;

	void Action();

	void CheckDistance();

	void OnTriggerEnter(ComponentCollider* collider) override;

	void OnAnimationEnd(const char* name) override;

	void OnDrawGizmosSelected(); 

	void PlaySFX(const char* sfx_name);
	bool IsState(const char* state_str) override;
// Group tactics
	//void OnGroupStrengthChange(float strength_multi) override;

// = = = = = = = = = = = = = = = = = = = = = = AWAKE BEHAVIOURS = = = = = = = = = = = = = = = = = = = = = =  
public:
	void DoAwake(); 
	void Dying(); 

private: 
	float current_wander_time = 0.f;
	float3 lastWanderTargetPos;
	bool curve_patrol_go = true; 
	float current_curve_point = 0.f;
public: 
	float curve_speed = 0.02f;
	bool patrol = false;
	float wander_speed = 2.0f; 
	float wander_radius = 0.0f;
	float wander_precision = 0.5f; 
	float wander_rest_time = 1.f;
	bool wander_rest = true; 
	GhoulState state = GhoulState::AWAKE;
	GhoulType ghoul_type = GhoulType::NONE;
	AwakeBehaviour awake_behaviour = AwakeBehaviour::DEFAULT;
	MusicController* m_controller = nullptr;
	GameObject* awake_curve = nullptr;
};


