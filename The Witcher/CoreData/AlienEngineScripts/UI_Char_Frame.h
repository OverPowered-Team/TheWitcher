#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API UI_Char_Frame : public Alien {

public:

	UI_Char_Frame();
	virtual ~UI_Char_Frame();
	
	void Start();

	void LifeChange(float life_change, float max_life);
	void ManaChange(float mana_change, float max_mana);
	void XpChange(float xp_change, float max_xp);
	
	
	enum(CHARACTER,
		GERALT,
		YENNEFER,
		);

	CHARACTER character = CHARACTER::GERALT;

	GameObject* geralt_img = nullptr;
	GameObject* yen_img = nullptr;
	GameObject* lifebar = nullptr;
	GameObject* mana_bar = nullptr;
	GameObject* xp_bar = nullptr;

	ComponentBar* lifebar_comp = nullptr;
	ComponentBar* mana_bar_comp = nullptr;
	ComponentBar* xp_bar_comp = nullptr;
};

ALIEN_FACTORY UI_Char_Frame* CreateUI_Char_Frame() {
	UI_Char_Frame* alien = new UI_Char_Frame();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_ENUM(UI_Char_Frame::CHARACTER, alien->character);
	return alien;
} 
