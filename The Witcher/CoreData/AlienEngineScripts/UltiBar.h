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
	void MaxBar();

	void ControlsLerp();

	GameObject* ultibar = nullptr;
	GameObject* ultibar_charged = nullptr;
	ComponentBar* ultimate_bar = nullptr;
	GameObject* controls = nullptr;
	GameObject* controls_lb = nullptr;
	GameObject* controls_rb = nullptr;
	ComponentImage* lb_image = nullptr;
	ComponentImage* rb_image = nullptr;

	ComponentBar* normal_ulti = nullptr;
	ComponentBar* charged_ulti = nullptr;

	float time_to_lerp = 0.5f;
	int times_shine = 3;

private:
	int shining_count = 0;
	float actual_time = 0.0f;
	bool glowing = true;
	float glow_time = 0.0f;
	bool controls_lerping = false;
	float pre_scale = 0.0f;
};

ALIEN_FACTORY UltiBar* CreateUltiBar() {
	UltiBar* alien = new UltiBar();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->time_to_lerp);
	SHOW_IN_INSPECTOR_AS_INPUT_INT(alien->times_shine);

	return alien;
} 
