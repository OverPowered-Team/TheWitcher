#pragma once

#include "..\..\Alien.h"
#include "Macros/AlienScripts.h"
#include "Stat.h"

enum (EnemyType,
	NONE = -1,
	GHOUL,
	NILFGAARD_SOLDIER,
	LESHEN
	);

class PlayerController;

class Enemy : public Alien {

public: 

	struct EnemyStats {
		float max_health = 0.0F;
		float current_health = 0.0F;
		Stat agility;
		float damage = 0.0F;
		float attack_speed = 0.0F;
		float attack_range = 0.0F;
		float vision_range = 0.0F;
	};

	enum (EnemyState,
		NONE = -1,
		IDLE,
		MOVE,
		ATTACK,
		HIT,
		BLOCK,
		FLEE,
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
	virtual void Move(float3 direction) {}
	virtual void Attack() {}
	void ActivateCollider();
	void DeactivateCollider();

	void OnTriggerEnter(ComponentCollider* collider);
	virtual void OnDeathHit() {}

	virtual float GetDamaged(float dmg, PlayerController* player);
	void ApplyEffects();

public:

	EnemyType type = EnemyType::NONE;
	EnemyStats stats;
	EnemyState state = EnemyState::NONE;
	ComponentAnimator* animator = nullptr;
	ComponentCharacterController* character_ctrl = nullptr;
	ComponentCollider* attack_collider = nullptr;
	bool can_get_interrupted = true;

	std::vector<PlayerController*> player_controllers;

	GameObject* player_1 = nullptr;
	GameObject* player_2 = nullptr;

	std::map<std::string, GameObject*> particles;
};