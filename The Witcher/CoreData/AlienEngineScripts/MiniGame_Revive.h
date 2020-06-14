#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class PlayerController;
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
	void LerpsOnInput();

	void StartMinigame(PlayerController* player_reviving);
	void EndMinigame();
	void RestartMinigame();

	States revive_state = States::PREGAME;
	float lerp_time = 1.0f;
	int input_times = 4;
	float revive_percentatge = 0.0f;
	PlayerController* player_dead = nullptr;
	PlayerController* player_reviving = nullptr;

private:

	void HeartPumPum();

private:
	GameObject* moving_part = nullptr;
	GameObject* minigame = nullptr;
	GameObject* start_X = nullptr;
	GameObject* game_A = nullptr;
	GameObject* good_part = nullptr;
	GameObject* heart = nullptr;

	int sign = 1;
	int actual_inputs = 0;
	float color_time = 0.0f;
	float time = 0.0f;
	bool effects_change = false;

	float position_goal = 2.0f;
	float original_moving_position = 0.0f;

	bool heart_pumpum = false;
	float heart_time = 0.0f;

	float sound_time = 0.0f;
	bool first_post = true;

	// Green Part reduction
	int correct_inputs = 0;
	float previous_scale = 0.0f;
	float new_scale = 0.0f;
	bool green_reducing = false;

	bool first_frame = true;
};

ALIEN_FACTORY MiniGame_Revive* CreateMiniGame_Revive() {
	MiniGame_Revive* alien = new MiniGame_Revive();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->lerp_time);
	SHOW_IN_INSPECTOR_AS_INPUT_INT(alien->input_times);

	return alien;
} 
