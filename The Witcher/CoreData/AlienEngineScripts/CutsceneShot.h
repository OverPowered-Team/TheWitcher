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
	float shake_time = 1.f;
	float shake_timer = 0.f;
	bool has_shaked = false;
};
struct CurveInfo {
	GameObject* path = nullptr;
	GameObject* target = nullptr;
	ComponentCurve* curve = nullptr;
	ComponentCamera* cam = nullptr;
	float3 start_pos = float3::zero();
	float3 start_front = float3::zero();
	float3 start_curve = float3::zero();
	float3 end_front = float3::zero();
	float3 end_curve = float3::zero();
	float3 front_end = float3::zero();

	float speed = 0;
	float current_pos = 0;
	bool start_transition = true;
	bool go_back = false;
	bool ended_intro = false;
	float transition_duration = 0;
	float current_time_transition = 0;
	bool entered_go_back = true;
};
struct CutsceneElement {
	int id_shot = 0;
	float transition_speed = 0.f;
	float stay_time = 1.f;
	float stay_timer = 0.f;
	bool it_shake = false;
	ShakeInfo info_shake;
	bool it_focus = false;
	GameObject* g_o_focus = nullptr;
	bool first_frame = true;
	Quat first_rot;
	Quat final_rot;
	float3  first_pos;
	bool it_curve = false;
	CurveInfo curve_info;
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
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->element.info_shake.shake_time);
	SHOW_SEPARATOR();
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->element.it_focus);
	SHOW_TEXT("Only fill the below field if it will focus some place");
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->element.g_o_focus);
	SHOW_SEPARATOR();
	SHOW_TEXT("Only fill the below field if it will curve some place");
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->element.it_curve);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->element.curve_info.speed);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->element.curve_info.transition_duration);

	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->element.curve_info.path);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->element.curve_info.target);

	return alien;
} 
