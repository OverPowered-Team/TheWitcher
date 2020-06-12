#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class PlayerController;
class UltiBar;
class DialogueManager;

enum(CP_STATE,
	FADE_IN,
	SHOW,
	FADE_OUT);

class UI_Particles
{
public:
	float3 final_position = float3(0, 0, 0);
	float3 origin_position = float3(0, 0, 0);
	GameObject* particle = nullptr;
	PlayerController* player = nullptr;
	float time_passed = 0.0f;
};

class ALIEN_ENGINE_API InGame_UI : public Alien {

public:

	InGame_UI();
	virtual ~InGame_UI();
	
	void Start();
	void Update();

	void PauseMenu(bool to_open);
	void RelicsMenu(bool to_open);
	
	void YouDied();
	void ShowCheckpointSaved();

	void StartLerpParticleUltibar(const float3& world_position);

	void ShowUltiFilter(bool show);

private:

	void LoadActiveRelics();

public:

	GameObject* in_game = nullptr;
	GameObject* pause_menu = nullptr;
	GameObject* relics_menu = nullptr;

	Prefab ulti_particle;
	Prefab killcount_particle;

	float time_lerp_ult_part = 1.5f;
	float ulti_filter_alpha = 0.2f;
	float time_paused = 0.0f;

private:

	bool died = false;
	bool died_gone = false;

	float time = 0.0f;
	int waiting = 1;

	GameObject* you_died = nullptr;
	GameObject* relics_panel = nullptr;
	ComponentCanvas* canvas = nullptr;

	ComponentImage* ulti_filter = nullptr;
	bool changing_alpha_filter = false;
	float time_ulti_filter = 0.0f;
	bool ulti_active = false;

	bool pause_active = false;

	// Checkpoint Saved Text
	GameObject* checkpoint_saved_text = nullptr;
	ComponentText* component_checkpoint_saved_text = nullptr;
	CP_STATE checkpoint_state = CP_STATE::FADE_IN;
	float time_checkpoint = 0.0f;

	// Charging ultibar particles
	std::vector<UI_Particles*> particles;
	UltiBar* ulti_bar = nullptr;

	// Timer
	float internal_timer = 0.0f;
};

ALIEN_FACTORY InGame_UI* CreateInGame_UI() {
	InGame_UI* alien = new InGame_UI();
	// To show in inspector here
	
	SHOW_IN_INSPECTOR_AS_PREFAB(alien->ulti_particle);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->time_lerp_ult_part);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->ulti_filter_alpha);

	return alien;
} 
