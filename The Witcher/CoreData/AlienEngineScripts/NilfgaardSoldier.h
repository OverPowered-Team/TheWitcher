#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Enemy.h"

class MusicController;

enum(NilfgaardSoldierState,
	NONE = -1,
	IDLE,
	MOVE,
	ATTACK,
	AUXILIAR,
	HIT,
	DYING,
	DEAD
	);

class ALIEN_ENGINE_API NilfgaardSoldier : public Enemy {
public:
	enum(NilfgaardType,
		NONE = -1,
		SWORD = 0,
		ARCHER = 1,
		SWORD_SHIELD = 2,
		MAX_NILFGAARD
		);

	NilfgaardSoldier() {}
	virtual ~NilfgaardSoldier() {}

	void StartEnemy() override;
	void SetStats(const char* json) override;
	float GetDamaged(float dmg, PlayerController* player);
	void UpdateEnemy() override;
	void CleanUpEnemy() override;

	void OnDeathHit();
	void CheckDistance();

	void OnAnimationEnd(const char* name) override;
	void OnTriggerEnter(ComponentCollider* collider);

public:
	Prefab head_prefab;
	GameObject* head_position;
	NilfgaardType nilf_type = NilfgaardType::NONE;
	NilfgaardSoldierState state = NilfgaardSoldierState::NONE;

protected:
	GameObject* decapitated_head = nullptr;
};

ALIEN_FACTORY NilfgaardSoldier* CreateNilfgaardSoldier() {
	NilfgaardSoldier* nilfgaard = new NilfgaardSoldier();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(NilfgaardSoldier::NilfgaardType, nilfgaard->nilf_type);
	SHOW_IN_INSPECTOR_AS_PREFAB(nilfgaard->head_prefab);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(nilfgaard->head_position);
	SHOW_VOID_FUNCTION(NilfgaardSoldier::ActivateCollider, nilfgaard);
	SHOW_VOID_FUNCTION(NilfgaardSoldier::DeactivateCollider, nilfgaard);

	return nilfgaard;
}


