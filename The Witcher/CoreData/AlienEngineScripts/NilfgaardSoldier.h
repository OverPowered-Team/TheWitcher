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
	void CleanUp() override;

	void OnAnimationEnd(const char* name) override;



public:
	GameObject* weapon = nullptr;
	NilfgaardType nilf_type = NilfgaardType::NONE;
	float distance = 0.0F;
};

ALIEN_FACTORY NilfgaardSoldier* CreateNilfgaardSoldier() {
	NilfgaardSoldier* alien = new NilfgaardSoldier();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(Enemy::EnemyState, alien->state);
	SHOW_IN_INSPECTOR_AS_ENUM(NilfgaardSoldier::NilfgaardType, alien->nilf_type);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->weapon);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->player_1);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->player_2);
	return alien;
}


