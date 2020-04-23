#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API MeleeLeshen : public Alien {

public:

	MeleeLeshen();
	virtual ~MeleeLeshen();
	
	void Start();
	void Update();

};

ALIEN_FACTORY MeleeLeshen* CreateMeleeLeshen() {
	MeleeLeshen* alien = new MeleeLeshen();
	// To show in inspector here

	return alien;
} 
