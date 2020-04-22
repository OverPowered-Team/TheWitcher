#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API CloudLeshen : public Alien {

public:

	CloudLeshen();
	virtual ~CloudLeshen();
	
	void Start();
	void Update();

};

ALIEN_FACTORY CloudLeshen* CreateCloudLeshen() {
	CloudLeshen* alien = new CloudLeshen();
	// To show in inspector here

	return alien;
} 
