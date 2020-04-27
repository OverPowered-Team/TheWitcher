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

ALIEN_FACTORY NilfSoldierMelee* CreateNilfSoldierMelee() {
	NilfSoldierMelee* alien = new NilfSoldierMelee();
	// To show in inspector here

	return alien;
} 
