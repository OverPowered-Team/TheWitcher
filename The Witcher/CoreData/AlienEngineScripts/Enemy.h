#pragma once

#include "..\..\Alien.h"
#include "Macros/AlienScripts.h"
#include "Stat.h"

#define ENEMY_JSON "GameData/Enemies/"

class PlayerController;
class Effect;
class SteeringAvoid;

enum (EnemyType,
	NONE = -1,
	GHOUL,
	NILFGAARD_SOLDIER,
	LESHEN,
	CIRI,
	CIRI_CLONE,
	DROWNED,
	SHAELMAR,
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
	virtual void Guard();
	virtual void CheckDistance() {};
	virtual void Action() {}

	void ActivateCollider();
	void DeactivateCollider();
	Quat RotateProjectile();

	virtual void PlaySFX(const char* sfx_name) {}

	virtual void Stun(float time) {};
	virtual void SetState(const char* state) {};
	virtual bool IsDead() { LOG("Calling virtual function of IsDead!"); return false; };
	virtual void Decapitate(PlayerController* player);

	virtual void OnTriggerEnter(ComponentCollider* collider) {};
	virtual void OnDeathHit() {}

	virtual float GetDamaged(float dmg, PlayerController* player, float3 knock_back = float3::zero());
	virtual float GetDamaged(float dmg, float3 knock_back = float3::zero());
	void AddEffect(Effect* new_effect);
	void RemoveEffect(Effect* _effect);

	void HitFreeze(float freeze_time);
	void SpawnAttackParticle();
	void StopHitFreeze(float speed);
	void SpawnParticle(std::string particle_name, float3 pos = float3::zero(), bool local = false, float3 rotation = float3::zero(), GameObject* parent = nullptr);
	void ReleaseParticle(std::string particle_name);
	void ReleaseAllParticles();

	void ChangeAttackEnemy(bool deleting = false);
	void RemoveBattleCircle();
	void AddBattleCircle(PlayerController* player_controller);
	void AddAttacking(PlayerController* player_controller);
	void RemoveAttacking(PlayerController* player_controller);

public:
	float distance = 0.0F;
	float3 direction; 
	float3 start_pos;
	float3 velocity = float3::zero();
	float knock_slow = -4.2f;
	float increase_hit_animation = 1.0f;
	float gravity = -20.0f;

	EnemyType type = EnemyType::NONE;
	ComponentAnimator* animator = nullptr;
	ComponentCharacterController* character_ctrl = nullptr;
	ComponentCollider* attack_collider = nullptr;
	ComponentAudioEmitter* audio_emitter = nullptr;
	SteeringAvoid* steeringAvoid = nullptr;
	bool can_get_interrupted = true;

	std::vector<PlayerController*> player_controllers;

	std::vector<GameObject*> particles;
	std::map<std::string, Stat> stats;

	bool is_frozen = false;
	bool is_combat = false;
	bool is_attacking = false;
	bool is_battle_circle = false;

	Prefab head_prefab;

protected:
	std::vector<GameObject*> particle_spawn_positions;
	std::vector<Effect*> effects;
	PlayerController* last_player_hit;
	GameObject* decapitated_head = nullptr;
	float current_stun_time = 0.0f;
	float stun_time = 0.0f;

	int current_player = 0;
};