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

	void StartEnemy() override;
	void UpdateEnemy() override;

	void SetStats(const char* json) override;
	void Attack() override;
	void Flee(float3 direction);

	void ShootAttack();
	Quat RotateArrow();
	void CleanUpEnemy() override;

	void OnAnimationEnd(const char* name) override;

public:
	Prefab arrow;
	NilfgaardType nilf_type = NilfgaardType::NONE;
};

ALIEN_FACTORY NilfgaardSoldier* CreateNilfgaardSoldier() {
	NilfgaardSoldier* nilfgaard = new NilfgaardSoldier();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(Enemy::EnemyState, nilfgaard->state);
	SHOW_IN_INSPECTOR_AS_ENUM(NilfgaardSoldier::NilfgaardType, nilfgaard->nilf_type);
	SHOW_IN_INSPECTOR_AS_PREFAB(nilfgaard->arrow);
	SHOW_VOID_FUNCTION(NilfgaardSoldier::ShootAttack, nilfgaard);
	SHOW_VOID_FUNCTION(NilfgaardSoldier::ActivateCollider, nilfgaard);
	SHOW_VOID_FUNCTION(NilfgaardSoldier::DeactivateCollider, nilfgaard);

	return nilfgaard;
}


