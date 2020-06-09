#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class Spawner;

class ALIEN_ENGINE_API Scale_Win : public Alien {

public:

	Scale_Win();
	virtual ~Scale_Win();
	
	void Start();
	void Update();

	void CalculateInclination();

	void LerpingText();
	void Scale();

	void HandleSceneLoad();

public:

	float max_Y = 4.5f;
	float time_to_scale = 0.5f;

	// Scores
	int player1_points = 0;
	int player2_points = 0;

	// Lerp
	int current_points1 = 0;
	int current_points2 = 0;

private:

	std::vector<int> ConvertKillsMapToVector(const std::map<uint, uint>& kills_map);

	void SpawnHeads(const std::vector<int>& kills, const std::vector<float2>& spawn_points, int player);

private:
	// Plates
	GameObject* left_scale = nullptr;
	GameObject* right_scale = nullptr;
	ComponentRigidBody* rigid_body1 = nullptr;
	ComponentRigidBody* rigid_body2 = nullptr;
	GameObject* connector = nullptr;

	// Spawners
	Spawner* spawner_l = nullptr;
	Spawner* spawner_r = nullptr;

	// Position
	float original_position1 = 0.0f;
	float original_position2 = 0.0f;
	float desired_position1 = 0.0f;
	float desired_position2 = 0.0f;
	float start_position1 = 0.0f;
	float start_position2 = 0.0f;
	bool in_place = true;

	// Scores
	ComponentText* score_text_1 = nullptr;
	ComponentText* score_text_2 = nullptr;

	// Time
	float time = 0.0f;

	bool first_frame = true;
	bool spawned_invoke = false;
};

ALIEN_FACTORY Scale_Win* CreateScale_Win() {
	Scale_Win* alien = new Scale_Win();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->max_Y);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->time_to_scale);

	return alien;
} 
