#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API MoreEventsClass : public Alien {

public:

	MoreEventsClass();
	virtual ~MoreEventsClass();
	
	void Start();
	void Update();


};

ALIEN_FACTORY MoreEventsClass* CreateMoreEventsClass() {
	MoreEventsClass* alien = new MoreEventsClass();
	// To show in inspector here

	return alien;
} 
