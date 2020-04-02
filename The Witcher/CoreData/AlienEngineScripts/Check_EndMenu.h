#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Check_EndMenu : public Alien {

public:

	Check_EndMenu();
	virtual ~Check_EndMenu();
	
	void Start();
	void Update();

};

ALIEN_FACTORY Check_EndMenu* CreateCheck_EndMenu() {
	Check_EndMenu* alien = new Check_EndMenu();
	// To show in inspector here

	return alien;
} 
