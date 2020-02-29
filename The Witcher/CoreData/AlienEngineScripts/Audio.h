#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Audio : public Alien {

public:

	Audio();
	virtual ~Audio();
	
	void Start();
	void Update();

	ComponentAudioEmitter* audio = nullptr;
};

ALIEN_FACTORY Audio* CreateAudio() {
	Audio* alien = new Audio();
	// To show in inspector here

	return alien;
} 
