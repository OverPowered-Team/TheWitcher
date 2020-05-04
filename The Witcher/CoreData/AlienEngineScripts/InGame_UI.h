#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class PlayerController;
class UltiBar;

enum(UI_Particle_Type,
	ULTI,
	KILL_COUNT,

	ANY = -1
	);

class UI_Particles
{
public:
	float3 final_position = float3(0, 0, 0);
	float3 origin_position = float3(0, 0, 0);
	GameObject* particle = nullptr;
	PlayerController* player = nullptr;
	float time_passed = 0.0f;
	UI_Particle_Type type = UI_Particle_Type::ANY;
};

class ALIEN_ENGINE_API InGame_UI : public Alien {

public:

	InGame_UI();
	virtual ~InGame_UI();
	
	void Start();
	void Update();

	void PauseMenu(bool to_open);
	
	void YouDied();
	void ShowCheckpointSaved();

	void StartLerpParticle(const float3& world_position, UI_Particle_Type type, PlayerController* player = nullptr);

public:

	GameObject* in_game = nullptr;
	GameObject* pause_menu = nullptr;

	Prefab ulti_particle;
	Prefab killcount_particle;

	float time_lerp_ult_part = 0.75f;

private:

	bool died = false;
	bool died_gone = false;

	float time = 0.0f;
	int waiting = 1;

	GameObject* you_died = nullptr;
	GameObject* relics_panel = nullptr;
	ComponentCanvas* canvas = nullptr;

	bool pause_active = false;

	// Checkpoint Saved Text
	GameObject* checkpoint_saved_text = nullptr;
	float time_checkpoint = 0.0f;

	// Charging ultibar particles
	std::vector<UI_Particles*> particles;
	UltiBar* ulti_bar = nullptr;
};

ALIEN_FACTORY InGame_UI* CreateInGame_UI() {
	InGame_UI* alien = new InGame_UI();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->in_game);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->pause_menu);
	
	SHOW_IN_INSPECTOR_AS_PREFAB(alien->ulti_particle);
	SHOW_IN_INSPECTOR_AS_PREFAB(alien->killcount_particle);

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->time_lerp_ult_part);

	return alien;
} 
