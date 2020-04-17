#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class CheckPoint;

class ALIEN_ENGINE_API CheckPoints_Manager : public Alien {

public:

	CheckPoints_Manager();
	virtual ~CheckPoints_Manager();
	
	void Start();
	void Update();

	void TP(int i);

	static std::vector<CheckPoint*> checkpoints;
	CheckPoint* last_checkpoint = nullptr;
};

ALIEN_FACTORY CheckPoints_Manager* CreateCheckPoints_Manager() {
	CheckPoints_Manager* alien = new CheckPoints_Manager();
	// To show in inspector here

	return alien;
} 
