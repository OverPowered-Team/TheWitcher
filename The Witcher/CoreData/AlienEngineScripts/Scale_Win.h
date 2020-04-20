#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Scale_Win : public Alien {

public:

	Scale_Win();
	virtual ~Scale_Win();
	
	void Start();
	void Update();

	void CalculateInclination();

	// Scales
	GameObject* left_scale = nullptr;
	GameObject* right_scale = nullptr;

	// Scores
	int player1_points = 0;
	int player2_points = 0;
	int* max_points = nullptr;
	int* min_points = nullptr;

	int current_points1 = 0;
	int current_points2 = 0;

	bool in_place = true;

	// Position
	float max_Y = 4.5f;
	float original_position1 = 0.0f;
	float original_position2 = 0.0f;
	float desired_position1 = 0.0f;
	float desired_position2 = 0.0f;

	// Time
	float time_to_scale = 0.0f;
	float time = 0.0f;
};

ALIEN_FACTORY Scale_Win* CreateScale_Win() {
	Scale_Win* alien = new Scale_Win();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_INPUT_INT(alien->player1_points);
	SHOW_IN_INSPECTOR_AS_INPUT_INT(alien->player2_points);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->max_Y);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->time_to_scale);

	return alien;
} 
