#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Main_Menu_Music : public Alien {

public:

	Main_Menu_Music();
	virtual ~Main_Menu_Music();
	
	void Start();

};

ALIEN_FACTORY Main_Menu_Music* CreateMain_Menu_Music() {
	Main_Menu_Music* alien = new Main_Menu_Music();
	// To show in inspector here

	return alien;
} 
