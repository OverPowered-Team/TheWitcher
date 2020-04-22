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

public:

	float max_Y = 4.5f;
	float time_to_scale = 0.5f;

	// Scores
	int player1_points = 0;
	int player2_points = 0;

private:
	// Scales
	GameObject* left_scale = nullptr;
	GameObject* right_scale = nullptr;
	ComponentRigidBody* rigid_body1 = nullptr;
	ComponentRigidBody* rigid_body2 = nullptr;

	// Position
	float original_position1 = 0.0f;
	float original_position2 = 0.0f;
	float desired_position1 = 0.0f;
	float desired_position2 = 0.0f;
	bool in_place = true;

	// Time

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
