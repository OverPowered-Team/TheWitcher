#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API CameraShake : public Alien {

public:

	CameraShake();
	virtual ~CameraShake();
	
	void Start();
	void Update();

	/*strength must be between 0 and 1*/
	void Shake(float strength);
	void Shake(float strength, const float& traumaDecay);

	void RepeatShake(float strength, float seconds_to_first_invoke, 
		float seconds_between_invokes, float time_to_stop);

	float traumaDecayDef = 0.9f;

	float maxYaw = 1.f;
	float maxPitch = 1.f;
	float maxRoll = 1.f;
	float off_set = 5.f;

private:
	float trauma = 0.f;
	float traumaDecay = 0.9f;

	float time_to_stop = 0.f;
	float invoke_timer = 0.f;

	Quat preQuat = Quat::identity();
	float shake_offset = 0.f;
	float post_off_set = 0.f;
};

ALIEN_FACTORY CameraShake* CreateCameraShake() {
	CameraShake* alien = new CameraShake();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(alien->traumaDecayDef, 0.001f, 1.0f);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->maxYaw);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->maxPitch);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->maxRoll);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->off_set);

	return alien;
} 
