#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API VagoneteObstacle : public Alien {

public:

	VagoneteObstacle();
	virtual ~VagoneteObstacle();
	
	void Start();
	void Update();

	bool isObstacleRight = true;

};

ALIEN_FACTORY VagoneteObstacle* CreateVagoneteObstacle() {
	VagoneteObstacle* alien = new VagoneteObstacle();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->isObstacleRight);
	return alien;
} 
