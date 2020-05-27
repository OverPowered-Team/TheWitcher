#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API MoveCameraArroundObject : public Alien {

public:
	GameObject* path = nullptr;
	GameObject* target = nullptr;
	ComponentCurve* curve = nullptr;
	ComponentCamera* camera = nullptr;
	float3 start_pos = float3::zero();
	float3 start_front = float3::zero();
	float3 start_curve = float3::zero();
	float3 end_front = float3::zero();
	float3 end_curve = float3::zero();

	float speed = 0;
	float current_pos = 0; 
	bool start_transition = true;
	bool go_back = false;
	bool ended_intro = false;
	float transition_duration = 0;
	float current_time_transition = 0;

	MoveCameraArroundObject();
	virtual ~MoveCameraArroundObject();
	
	void Start();
	void Update();

};

ALIEN_FACTORY MoveCameraArroundObject* CreateMoveCameraArroundObject() {
	MoveCameraArroundObject* alien = new MoveCameraArroundObject();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->speed);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->transition_duration);

	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->path);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->target);

	return alien;
} 
