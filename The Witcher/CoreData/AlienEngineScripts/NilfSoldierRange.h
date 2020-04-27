#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "NilfgaardSoldier.h"

class ALIEN_ENGINE_API NilfSoldierRange : public NilfgaardSoldier {
public:

	NilfSoldierRange();
	virtual ~NilfSoldierRange();
	
	void UpdateEnemy() override;
	void Action() override;

	void Flee();
	void ShootAttack();
	Quat RotateArrow();

public:
	Prefab arrow;

};

ALIEN_FACTORY NilfSoldierRange* CreateNilfSoldierRange() {
	NilfSoldierRange* nilfgaard = new NilfSoldierRange();

	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_PREFAB(nilfgaard->arrow);
	SHOW_VOID_FUNCTION(NilfSoldierRange::ShootAttack, nilfgaard);
	return nilfgaard;
} 
