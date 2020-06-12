#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API TestingThread : public Alien {

public:

	TestingThread();
	virtual ~TestingThread();
	
	void Start();
	void Update();

};

ALIEN_FACTORY TestingThread* CreateTestingThread() {
	TestingThread* alien = new TestingThread();
	// To show in inspector here

	return alien;
} 
