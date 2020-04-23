#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class CheckPoints_Manager;

class ALIEN_ENGINE_API CheckPoint : public Alien {

public:

	CheckPoint();
	virtual ~CheckPoint();
	
	void Awake();
	void OnDrawGizmos();
};

ALIEN_FACTORY CheckPoint* CreateCheckPoint() {
	CheckPoint* alien = new CheckPoint();
	// To show in inspector here

	return alien;
} 
