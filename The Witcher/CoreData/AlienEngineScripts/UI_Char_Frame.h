#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API UI_Char_Frame : public Alien {

public:

	UI_Char_Frame();
	virtual ~UI_Char_Frame();
	
	void Start();
	void Update();

	void LifeChange(float life_change);
	void ManaChange(float mana_change);
	void XpChange(float xp_change);
	
	
	enum(CHARACTER,
		GERALT,
		YENNEFER,
		);

	CHARACTER character = CHARACTER::GERALT;

	GameObject* geralt_img = nullptr;
	GameObject* yen_img = nullptr;
	ComponentBar* lifebar = nullptr;
	ComponentBar* mana_bar = nullptr;
	ComponentBar* xp_bar = nullptr;

	// TODO VS3: CHANGE WITH PLAYER VALUES
	float max_life = 100.0f;
	float max_mana = 100.0f;
	float max_xp = 100.0f;
};

ALIEN_FACTORY UI_Char_Frame* CreateUI_Char_Frame() {
	UI_Char_Frame* alien = new UI_Char_Frame();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_ENUM(UI_Char_Frame::CHARACTER, alien->character);
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->max_life,0.0f,100.0f);
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->max_mana, 0.0f, 100.0f);
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->max_xp, 0.0f, 100.0f);
	return alien;
} 
