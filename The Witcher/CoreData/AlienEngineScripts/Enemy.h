#pragma once

#include "..\..\Alien.h"
#include "Macros/AlienScripts.h"


enum (EnemyType,
	NONE = -1,
	GHOUL,
	NILFGAARD_SOLDIER
	);

class Enemy : public Alien {

public: 

	struct EnemyStats {
		float health = 0.0F;
		float agility = 0.0F;
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
		BLOCK,
		DEAD,
		FLEE,
		);

public:

	Enemy();
	virtual ~Enemy();

	void Awake();

	/*-------CALLED BY ENEMY MANAGER--------*/
	virtual void StartEnemy() {}
	virtual void UpdateEnemy() {}
	virtual void CleanUpEnemy() {}
	/*-------CALLED BY ENEMY MANAGER--------*/

	virtual void Start();
	virtual void SetStats(const char* json);
	virtual void Move(float3 direction) {}
	virtual void Update() {}
	virtual void CleanUp() {}

public:
	EnemyType type = EnemyType::NONE;
	EnemyStats stats;
	EnemyState state = EnemyState::NONE;
	ComponentAnimator* animator = nullptr;
	ComponentCharacterController* character_ctrl = nullptr;

	GameObject* player_1 = nullptr;
	GameObject* player_2 = nullptr;
};