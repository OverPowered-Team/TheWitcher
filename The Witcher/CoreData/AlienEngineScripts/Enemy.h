#pragma once

#include "..\..\Alien.h"
#include "Macros/AlienScripts.h"
#include "Stat.h"

#define ENEMY_JSON "GameData/Enemies/"

class PlayerController;
class Effect;

enum (EnemyType,
	NONE = -1,
	GHOUL,
	NILFGAARD_SOLDIER,
	DROWNED,
	SHAELMAR,
	LESHEN,
	BLOCKER_OBSTACLE
	);

class Enemy : public Alien {
public:

	Enemy() {}
	virtual ~Enemy() {}

	void Awake();

	/*-------CALLED BY ENEMY MANAGER--------*/
	virtual void StartEnemy();
	virtual void UpdateEnemy();
	virtual void CleanUpEnemy();
	/*-------CALLED BY ENEMY MANAGER--------*/

	virtual void SetStats(const char* json);
	virtual void Move(float3 direction);
	virtual void CheckDistance() {};
	virtual void Action() {}
	void ActivateCollider();
	void DeactivateCollider();

	virtual void Stun(float time) {};
	virtual void KnockBack(float3 knock);
	virtual void SetState(const char* state) {};
	virtual bool IsDead() { LOG("Calling virtual function of IsDead!"); return false; };

	virtual void OnTriggerEnter(ComponentCollider* collider) {};
	virtual void OnDeathHit() {}

	virtual float GetDamaged(float dmg, PlayerController* player, float3 knock_back = float3::zero());
	void AddEffect(Effect* new_effect);
	void RemoveEffect(Effect* _effect);

	void HitFreeze(float freeze_time);
	void StopHitFreeze(float speed);

public:
	float distance = 0.0F;
	float3 direction;
	float3 velocity = float3::zero();
	float knock_slow = -4.2f;

	EnemyType type = EnemyType::NONE;
	ComponentAnimator* animator = nullptr;
	ComponentCharacterController* character_ctrl = nullptr;
	ComponentCollider* attack_collider = nullptr;
	ComponentAudioEmitter* audio_emitter = nullptr;
	bool can_get_interrupted = true;

	std::vector<PlayerController*> player_controllers;

	std::map<std::string, ComponentParticleSystem*> particles;
	std::map<std::string, Stat> stats;

	bool is_frozen = false;
	bool is_combat = false;

protected:
	std::vector<Effect*> effects;
	float current_stun_time = 0.0f;
	float stun_time = 0.0f;

	int current_player = 0;
};