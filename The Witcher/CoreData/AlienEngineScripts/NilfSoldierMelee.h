#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "NilfgaardSoldier.h"

class ALIEN_ENGINE_API NilfSoldierMelee : public NilfgaardSoldier {

public:

	NilfSoldierMelee();
	virtual ~NilfSoldierMelee();
	
	void UpdateEnemy() override;
	void Action() override;

};

ALIEN_FACTORY NilfgaardSoldier* CreateNilfSoldierMelee() {
	NilfSoldierMelee* nilfgaard = new NilfSoldierMelee();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(nilfgaard->increase_hit_animation);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(nilfgaard->gravity);
	SHOW_IN_INSPECTOR_AS_ENUM(NilfgaardSoldierState, nilfgaard->state);
	SHOW_IN_INSPECTOR_AS_ENUM(NilfgaardSoldier::NilfgaardType, nilfgaard->nilf_type);
	SHOW_IN_INSPECTOR_AS_PREFAB(nilfgaard->head_prefab);
	SHOW_VOID_FUNCTION(Enemy::SpawnAttackParticle, nilfgaard);
	SHOW_VOID_FUNCTION(NilfgaardSoldier::ActivateCollider, nilfgaard);
	SHOW_VOID_FUNCTION(NilfgaardSoldier::DeactivateCollider, nilfgaard);

	return nilfgaard;
}
