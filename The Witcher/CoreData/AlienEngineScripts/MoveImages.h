#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API MoveImages : public Alien {

public:

	MoveImages();
	virtual ~MoveImages();
	
	void Start();
	void Update();
public:
	GameObject* nextPos  = nullptr;
	float start_movement = 0;
	float start_time = 0;
	float current_time = 0;
	bool started_event = false;
	float transition_duration = 0.5;
}; 

ALIEN_FACTORY MoveImages* CreateMoveImages() {
	MoveImages* alien = new MoveImages();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->nextPos, "Next Position");
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->start_movement);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->transition_duration);
	return alien;
} 
