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
	float time_to_explode = 8.f;
	bool has_exploded = false;
	ComponentAudioEmitter* emitter = nullptr;
};

ALIEN_FACTORY SadTutorial* CreateSadTutorial() {
	SadTutorial* alien = new SadTutorial();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->time_to_explode);

	return alien;
} 
