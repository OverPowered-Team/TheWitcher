#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Dwarf : public Alien {

public:

	Dwarf() {};
	virtual ~Dwarf() {};
	void Update();

	float speed = 5.f;

};

ALIEN_FACTORY Dwarf* CreateDwarf() {
	Dwarf* alien = new Dwarf();

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->speed);
	return alien;
}

