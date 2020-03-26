#pragma once

#include "Macros/AlienScripts.h"

class Enemy {

public: 

	struct EnemyStats {
		std::string weapon = "None";
		float health = 0.0F;
		float agility = 0.0F;
		float damage = 0.0F;
		float attack_speed = 0.0F;
	};

	enum (EnemyType,
		NONE = -1,
		GHOUL,
		NILFGAARD_SOLDIER
	);

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

	virtual void StartEnemy();
	virtual void SetStats(const char* json);
	virtual void Update() {}
	virtual void CleanUp() {}

public:
	EnemyType type = EnemyType::NONE;
	EnemyStats stats;
	EnemyState state = EnemyState::NONE;
	GameObject* game_object = nullptr;
	ComponentAnimator* animator = nullptr;
};