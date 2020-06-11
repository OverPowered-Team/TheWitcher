#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API SadTutorial : public Alien {

public:

	SadTutorial();
	virtual ~SadTutorial();
	
	void Start();
	void Update();

public:
	float timer_explosion = 0.f;
	ComponentAudioEmitter* emitter = nullptr;
};

ALIEN_FACTORY SadTutorial* CreateSadTutorial() {
	SadTutorial* alien = new SadTutorial();
	// To show in inspector here

	return alien;
} 
