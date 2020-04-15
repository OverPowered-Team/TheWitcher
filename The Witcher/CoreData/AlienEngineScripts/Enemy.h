#pragma once

#include "..\..\Alien.h"
#include "Macros/AlienScripts.h"
#include "Stat.h"

#define ENEMY_JSON "Configuration/Enemies/"

class PlayerController;
class Effect;

enum (EnemyType,
	NONE = -1,
	GHOUL,
	NILFGAARD_SOLDIER
	);

class Enemy : public Alien {

public: 
	enum (EnemyState,
		NONE = -1,
		IDLE,
		MOVE,
		ATTACK,
		HIT,
		BLOCK,
		FLEE,
		STUNNED,
		DYING,
		DEAD,
		);

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
	virtual void Attack() {}
	virtual void Action() {}
	void ActivateCollider();
	void DeactivateCollider();

	void OnTriggerEnter(ComponentCollider* collider);
	virtual void OnDeathHit() {}

	float GetDamaged(float dmg, PlayerController* player);
	void AddEffect(Effect* new_effect);

public:
	float distance = 0.0F;
	float3 direction;
	EnemyType type = EnemyType::NONE;
	EnemyState state = EnemyState::NONE;
	ComponentAnimator* animator = nullptr;
	ComponentCharacterController* character_ctrl = nullptr;
	ComponentCollider* attack_collider = nullptr;
	Prefab head_prefab;

	std::vector<PlayerController*> player_controllers;

	std::map<std::string, GameObject*> particles;
	std::map<std::string, Stat> stats;

protected:
	GameObject* decapitated_head = nullptr;
	std::vector<Effect*> effects;
};