#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API UI_Char_Frame : public Alien {

public:

	UI_Char_Frame();
	virtual ~UI_Char_Frame();
	
	void Start();
	void Update();

	void LifeChange(float life_change, float max_life);
	void ManaChange(float mana_change, float max_mana);
	void XpChange(float xp_change, float max_xp);
	
	enum(CHARACTER,
		GERALT,
		YENNEFER,
		);

	CHARACTER character = CHARACTER::GERALT;

	// Lerps
	float change_time = 0.150f;

private:
	float life_change = 0.0f;
	float max_life = 0.0f;

	// Lerps
	float now_life = 0.0f;
	float time = 0.0f;
	bool changing_life = false;

	// HUD Components
	GameObject* geralt_img = nullptr;
	GameObject* yen_img = nullptr;
	ComponentBar* lifebar = nullptr;
	ComponentBar* mana_bar = nullptr;
	ComponentBar* xp_bar = nullptr;
};

ALIEN_FACTORY UI_Char_Frame* CreateUI_Char_Frame() {
	UI_Char_Frame* alien = new UI_Char_Frame();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_ENUM(UI_Char_Frame::CHARACTER, alien->character);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->change_time);

	return alien;
} 
