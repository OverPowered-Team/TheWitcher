#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Enemy.h"

class ALIEN_ENGINE_API NilfgaardSoldier : public Enemy {
public:
	enum(NilfgaardType,
		NONE = -1,
		SWORD_SHIELD,
		SPEAR,
		LARGE_SWORD,
		ARCHER
		);

	NilfgaardSoldier() {}
	virtual ~NilfgaardSoldier() {}

	void Start() override;
	void Update() override;
	//void SetStats(const char* json) override;
	void Move(float3 direction) override;
	void CleanUp() override;

public:
	GameObject* weapon = nullptr;
	NilfgaardType nilf_type = NilfgaardType::NONE;
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


