#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Boss_Lifebar : public Alien {

public:

	Boss_Lifebar();
	virtual ~Boss_Lifebar();
	
private:
	void Start();
	void Update();

	void AlphaChange();

	void FillingBar();

public:

	void UpdateLifebar(float life, float max_life);

	void ShowLifeBar(bool want_to_show);

public:

	float time_to_lerp			= 0.25f;
	float time_to_show			= 0.5f;

private:

	// Components
	ComponentBar* lifebar		= nullptr;
	ComponentText* boss_name	= nullptr;

	// Effects
	bool lifebar_changing		= false;
	float start_lerp_time		= 0.0f;
	float goal_life				= 0.0f;
	float start_life			= 0.0f;

	// Showing Effect
	bool changing_alpha			= false;
	float start_time_show		= false;
	float starting_point		= false;
	float ending_point			= false;
	
	bool filling_bar			= false;
	float time_start_fill		= 0.0f;
	bool first_time_filling		= true;

};

ALIEN_FACTORY Boss_Lifebar* CreateBoss_Lifebar() {
	Boss_Lifebar* alien = new Boss_Lifebar();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->time_to_lerp);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->time_to_show);

	return alien;
} 
