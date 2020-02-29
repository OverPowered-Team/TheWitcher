#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API TestClass4 : public Alien {

public:

	TestClass4();
	virtual ~TestClass4();
	
	void Start();
	void Update();

};

ALIEN_FACTORY TestClass4* CreateTestClass4() {
	TestClass4* alien = new TestClass4();
	// To show in inspector here

	return alien;
} 
