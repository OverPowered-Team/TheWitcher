#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

struct ShakeInfo {
	float strength = 0.f;
	float traumaDecay = 0.f;
	float off_set = 0.f;
	float maxyaw = 1.f;
	float maxpitch = 1.f;
	float maxroll = 1.f;
};

struct CutsceneElement {
	int id_shot = 0;
	float transition_speed = 0.f;
	float play_time = 1.f;
	float play_timer = 0.f;
	float stay_time = 1.f;
	float stay_timer = 0.f;
	bool it_shake = false;
	ShakeInfo info_shake;
	bool it_focus = false;
	GameObject* g_o_focus = nullptr;
};

class ALIEN_ENGINE_API CutsceneShot : public Alien {

public:

	CutsceneShot();
	virtual ~CutsceneShot();
	
	void Start();
	void Update();

public:
	CutsceneElement element;
};

ALIEN_FACTORY CutsceneShot* CreateCutsceneShot() {
	CutsceneShot* alien = new CutsceneShot();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_SLIDER_INT(alien->element.id_shot, 0, 100);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->element.transition_speed);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->element.stay_time);
	SHOW_SEPARATOR();
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->element.it_shake);
	SHOW_TEXT("Only fill the below fields if the camera will shake");
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->element.info_shake.strength);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->element.info_shake.traumaDecay);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->element.info_shake.off_set);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->element.info_shake.maxyaw);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->element.info_shake.maxpitch);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->element.info_shake.maxroll);
	SHOW_SEPARATOR();
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->element.it_focus);
	SHOW_TEXT("Only fill the below field if it will focus some place");
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->element.g_o_focus);

	return alien;
} 
