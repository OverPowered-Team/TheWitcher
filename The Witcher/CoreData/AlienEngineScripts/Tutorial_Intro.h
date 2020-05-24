#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

enum (Tutorial_Intro_State,
	DIALOGUE_1,
	DIALOGUE_2,
	SHAKE,
	CAMERA_ASCENSION,
	PARTICLE_EXPLOSION,
	CAMERA_RETURN,
	FINISH,

	ANY = -1,
	);


class ALIEN_ENGINE_API Tutorial_Intro : public Alien {

public:

	Tutorial_Intro();
	virtual ~Tutorial_Intro();
	
private:

	void Start();
	void Update();

private:

	Tutorial_Intro_State current_state = Tutorial_Intro_State::ANY;

};

ALIEN_FACTORY Tutorial_Intro* CreateTutorial_Intro() {
	Tutorial_Intro* alien = new Tutorial_Intro();
	// To show in inspector here

	return alien;
} 
