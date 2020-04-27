#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

enum(States,
	PREGAME,
	MINIGAME,
	POSTGAME
	)

class ALIEN_ENGINE_API MiniGame_Revive : public Alien {

public:

	MiniGame_Revive();
	virtual ~MiniGame_Revive();
	
	void Start();
	void Update();

	void Minigame();

	void Effects();

	States revive_state = States::PREGAME;
	float lerp_time = 1.0f;
	int input_times = 4;
	float revive_percentatge = 0.0f;

private:
	GameObject* moving_part = nullptr;
	GameObject* minigame = nullptr;
	GameObject* start_X = nullptr;
	GameObject* text = nullptr;
	float color_time = 0.0f;
	float time = 0.0f;
	int sign = 1;
	int actual_inputs = 0;
	bool color_changed = false;
};

ALIEN_FACTORY MiniGame_Revive* CreateMiniGame_Revive() {
	MiniGame_Revive* alien = new MiniGame_Revive();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->lerp_time);
	SHOW_IN_INSPECTOR_AS_INPUT_INT(alien->input_times);

	return alien;
} 
