#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "NilfgaardSoldier.h"

class ALIEN_ENGINE_API NilfSoldierRange : public NilfgaardSoldier {
public:

	NilfSoldierRange();
	virtual ~NilfSoldierRange();
	
	void UpdateEnemy() override;
	void CheckDistance() override;
	void Action() override;

	void Flee();
	void ShootAttack();

public:
	Prefab arrow;
	float last_flee_distance = 0.0f;
	float flee_min_distance = 0.01f;
	float current_flee_distance = 0.0f;
};

ALIEN_FACTORY NilfSoldierRange* CreateNilfSoldierRange() {
	NilfSoldierRange* nilfgaard = new NilfSoldierRange();

	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(NilfgaardSoldierState, nilfgaard->state);
	SHOW_IN_INSPECTOR_AS_ENUM(NilfgaardSoldier::NilfgaardType, nilfgaard->nilf_type);
	SHOW_IN_INSPECTOR_AS_PREFAB(nilfgaard->head_prefab);
	SHOW_VOID_FUNCTION(NilfgaardSoldier::ActivateCollider, nilfgaard);
	SHOW_VOID_FUNCTION(NilfgaardSoldier::DeactivateCollider, nilfgaard);
	SHOW_IN_INSPECTOR_AS_PREFAB(nilfgaard->arrow);
	SHOW_VOID_FUNCTION(NilfSoldierRange::ShootAttack, nilfgaard);
	return nilfgaard;
} 
