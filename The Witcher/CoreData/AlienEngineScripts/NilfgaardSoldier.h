#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Enemy.h"

class ALIEN_ENGINE_API NilfgaardSoldier : public Enemy {
public:
	enum(NilfgaardType,
		NONE = -1,
		SWORD_SHIELD = 0,
		ARCHER = 1,
		SPEAR = 2,
		LARGE_SWORD = 3,
		MAX_NILFGAARD
		);

	NilfgaardSoldier() {}
	virtual ~NilfgaardSoldier() {}

	void Start() override;
	void Update() override;
	void SetStats(const char* json) override;
	void Move(float3 direction) override;
	void Attack() override;
	void ShootAttack();
	void CleanUp() override;

	void OnAnimationEnd(const char* name) override;



public:
	GameObject* weapon = nullptr;
	Prefab arrow;
	NilfgaardType nilf_type = NilfgaardType::NONE;
	float distance = 0.0F;
	float3 direction;
};

ALIEN_FACTORY NilfgaardSoldier* CreateNilfgaardSoldier() {
	NilfgaardSoldier* nilfgaard = new NilfgaardSoldier();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(Enemy::EnemyState, nilfgaard->state);
	SHOW_IN_INSPECTOR_AS_ENUM(NilfgaardSoldier::NilfgaardType, nilfgaard->nilf_type);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(nilfgaard->weapon);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(nilfgaard->player_1);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(nilfgaard->player_2);
	SHOW_IN_INSPECTOR_AS_PREFAB(nilfgaard->arrow);
	return nilfgaard;
}


