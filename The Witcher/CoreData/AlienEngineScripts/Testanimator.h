#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

#include "TestEvents.h"

class ALIEN_ENGINE_API Testanimator : public Alien {

public:

	Testanimator();
	virtual ~Testanimator();
	
	void Start();
	void Update();


	ComponentAnimator* anim = nullptr;

	TestEvents::MYENUM myEnumTest = TestEvents::MYENUM::two;
	MYENUM2 myEnumTest2 = MYENUM2::asdada;
};

ALIEN_FACTORY Testanimator* CreateTestanimator() {
	Testanimator* alien = new Testanimator();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(TestEvents::MYENUM, alien->myEnumTest);
	SHOW_IN_INSPECTOR_AS_ENUM(MYENUM2, alien->myEnumTest2);
	return alien;
} 
