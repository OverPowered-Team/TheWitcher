#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API TestSwitches : public Alien {

public:

	TestSwitches();
	virtual ~TestSwitches();

	void Start();
	void Update();


	ComponentAudioEmitter* emmitter = nullptr;
};

ALIEN_FACTORY TestSwitches* CreateTestSwitches() {
	TestSwitches* switches = new TestSwitches();
	// To show in inspector here
	return switches;
}

