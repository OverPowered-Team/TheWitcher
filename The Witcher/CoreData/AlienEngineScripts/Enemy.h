#pragma once

#include "..\..\Alien.h"
#include "Macros/AlienScripts.h"
#include "Stat.h"

class PlayerController;
class Effect;

enum (EnemyType,
	NONE = -1,
	GHOUL,
	NILFGAARD_SOLDIER
	);

class Enemy : public Alien {

public: 

	struct EnemyStats {
		Stat health;
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

	float GetDamaged(float dmg);

public:

	EnemyType type = EnemyType::NONE;
	EnemyStats stats;
	EnemyState state = EnemyState::NONE;
	ComponentAnimator* animator = nullptr;
	ComponentCharacterController* character_ctrl = nullptr;
	ComponentCollider* attack_collider = nullptr;

	std::vector<PlayerController*> player_controllers;

	GameObject* player_1 = nullptr;
	GameObject* player_2 = nullptr;

	std::map<std::string, GameObject*> particles;
	std::vector<Effect*> effects;
};