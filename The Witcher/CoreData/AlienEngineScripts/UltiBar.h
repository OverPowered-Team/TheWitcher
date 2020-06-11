#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API UltiBar : public Alien {

public:

	UltiBar();
	virtual ~UltiBar();
	
	void Start();
	void Update();

	void UpdateBar(float actual_value);

	void ControlsLerp();

	void UpdateTimes(float time_paused);

	// GameObjects
	GameObject* ultibar = nullptr;
	ComponentBar* normal_ulti = nullptr;

	// Effects
	GameObject* controls = nullptr;
	GameObject* controls_lb = nullptr;
	GameObject* controls_rb = nullptr;
	ComponentImage* lb_image = nullptr;
	ComponentImage* rb_image = nullptr;

	// Controls Lerp
	float time_to_lerp_controls = 0.5f;
	float ulti_increase_lerp_time = 0.1f;
	int times_shine = 3;

private:
	// Effects Data
	// Controls
	int shining_count = 0;
	float actual_time = 0.0f;
	bool controls_lerping = false;
	float pre_scale = 0.0f;

	// Ultibar Glowing
	bool glowing = true;
	float glow_time = 0.0f;
	bool is_max = false;
	bool is_bar_charged = false;

	// Ultibar Charging Lerp
	bool bar_charging = false;
	float new_bar_value = 0.0f;
	float previous_bar_value = 0.0f;
	float bar_charging_time = 0.0f;

	// Internal Timer
	float internal_timer = 0.0f;
};

ALIEN_FACTORY UltiBar* CreateUltiBar() {
	UltiBar* alien = new UltiBar();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->ulti_increase_lerp_time);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->time_to_lerp_controls);
	SHOW_IN_INSPECTOR_AS_INPUT_INT(alien->times_shine);

	return alien;
} 
