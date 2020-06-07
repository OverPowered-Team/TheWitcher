#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

enum(TYPE,
	PUSH,
	DAMAGE,
	BOTH,

	ANY);

enum(OSCILATION_DIRECTION,
	X,
	Y,
	Z);

class ALIEN_ENGINE_API Training_Zone : public Alien {

public:

	Training_Zone();
	virtual ~Training_Zone();
	
public:

	OSCILATION_DIRECTION oscilation_direction = OSCILATION_DIRECTION::X;
	TYPE osciling_type = TYPE::ANY;
	int initial_sign = 1;
	float cycle_time = 0.5f;
	float damage_to_do = 20.f;
	float push_force = 5.0f;

private:

	void Start();
	void Update();

	void OnTriggerEnter(ComponentCollider* col) override;

private:

	void Init();

private:

	ComponentRigidBody* rb = nullptr;

	float max_oscilation_pos = 0.0f;
	float current_oscilating_time = 0.0f;

	float internal_timer = 0.0f;
	float time_paused = 0.0f;

	bool has_started = false;

};

ALIEN_FACTORY Training_Zone* CreateTraining_Zone() {
	Training_Zone* alien = new Training_Zone();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_ENUM(OSCILATION_DIRECTION, alien->oscilation_direction);
	SHOW_IN_INSPECTOR_AS_ENUM(TYPE, alien->osciling_type);
	SHOW_IN_INSPECTOR_AS_SLIDER_INT(alien->initial_sign, -1, 1);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->cycle_time);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->damage_to_do);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->push_force);

	return alien;
} 
