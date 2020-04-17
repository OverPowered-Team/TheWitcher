#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API ExplodeChildren : public Alien {

public:

	ExplodeChildren();
	virtual ~ExplodeChildren();
	
	void Start();
	void Update();

	void DeleteMyself();

};

ALIEN_FACTORY ExplodeChildren* CreateExplodeChildren() {
	ExplodeChildren* alien = new ExplodeChildren();
	// To show in inspector here

	return alien;
} 
