#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API MiniGame_Revive : public Alien {

public:

	MiniGame_Revive();
	virtual ~MiniGame_Revive();
	
	void Start();
	void Update();

	GameObject* moving_part = nullptr;
	float lerp_time = 1.0f;
	int input_times = 4;
	float revive_percentatge = 0.0f;

private:
	GameObject* text = nullptr;
	float time = 0.0f;
	int sign = 1;
	int actual_inputs = 0;
};

ALIEN_FACTORY MiniGame_Revive* CreateMiniGame_Revive() {
	MiniGame_Revive* alien = new MiniGame_Revive();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->lerp_time);
	SHOW_IN_INSPECTOR_AS_INPUT_INT(alien->input_times);

	return alien;
} 
