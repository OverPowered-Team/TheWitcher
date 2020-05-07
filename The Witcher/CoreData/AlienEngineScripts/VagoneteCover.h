#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API VagoneteCover : public Alien {

public:

	VagoneteCover();
	virtual ~VagoneteCover();
	
	void Start();
	void Update();

};

ALIEN_FACTORY VagoneteCover* CreateVagoneteCover() {
	VagoneteCover* alien = new VagoneteCover();
	// To show in inspector here

	return alien;
} 
