#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Drowned.h"

class ALIEN_ENGINE_API DrownedRange : public Drowned {

public:

	DrownedRange();
	virtual ~DrownedRange();
	
	void UpdateEnemy() override;

	void Action() override;

	void OnAnimationEnd(const char* name) override;
};

ALIEN_FACTORY DrownedRange* CreateDrownedRange() {
	DrownedRange* alien = new DrownedRange();
	// To show in inspector here

	return alien;
} 
