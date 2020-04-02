#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API DebugManager : public Alien {

public:

	DebugManager();
	virtual ~DebugManager();
	
	void Start();
	void Update();

};

ALIEN_FACTORY DebugManager* CreateDebugManager() {
	DebugManager* alien = new DebugManager();
	// To show in inspector here

	return alien;
} 
