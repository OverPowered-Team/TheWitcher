#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

enum(KILL_COUNT_STATE,
	FADING_IN,
	SHOWING,
	FADING_OUT
	);

class ALIEN_ENGINE_API UI_Char_Frame : public Alien {

public:

	UI_Char_Frame();
	virtual ~UI_Char_Frame();
	
	void Start();
	void Update();

	void LifeChange(float life_change, float max_life);
	void ManaChange(float mana_change, float max_mana);

	void StartFadeKillCount(int new_kill_count);

public:

	enum(CHARACTER,
		GERALT,
		YENNEFER,
		);

	CHARACTER character = CHARACTER::GERALT;

	// Lerps
	float change_time = 0.150f;
	float killcount_time_to_lerp = 0.5f;

	// KillCount
	GameObject* kill_count = nullptr;
	ComponentText* kill_count_number = nullptr;
	ComponentText* kill_count_number_X = nullptr;

private:

	void HitEffect(float lerp_time);
	void LowLifeGlow();

private:
	// life
	float life_change = 0.0f;
	float max_life = 0.0f;
	bool player_hit = false;
	bool low_life = false;

	// Lerps life
	float now_life = 0.0f;
	float time = 0.0f;
	bool changing_life = false;
	float low_life_glow_time = 0.0f;
	int low_life_sign = 1;

	// Chaos
	float actual_chaos = 0.0f;
	float chaos_change = 0.0f;
	float max_chaos = 0.0f;
	float chaos_time = 0.0f;
	bool changing_chaos = false;

	// Kill Count
	bool is_showing_kill_count = false;
	float killcount_lerp_time = 0.0f;
	KILL_COUNT_STATE kc_state = KILL_COUNT_STATE::FADING_IN;

	// HUD Components
	GameObject* geralt_img = nullptr;
	GameObject* yen_img = nullptr;
	ComponentImage* portrait = nullptr;

	ComponentBar* lifebar = nullptr;
	ComponentBar* mana_bar = nullptr;
};

ALIEN_FACTORY UI_Char_Frame* CreateUI_Char_Frame() {
	UI_Char_Frame* alien = new UI_Char_Frame();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_ENUM(UI_Char_Frame::CHARACTER, alien->character);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->change_time);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->killcount_time_to_lerp);

	return alien;
} 
