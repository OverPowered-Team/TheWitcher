#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API MoveCameraArroundObject : public Alien {

public:
	GameObject* path;
	ComponentCurve* curve = nullptr;
	float speed = 0;
	float current_pos = 0; 
	MoveCameraArroundObject();
	virtual ~MoveCameraArroundObject();
	
	void Start();
	void Update();

};

ALIEN_FACTORY MoveCameraArroundObject* CreateMoveCameraArroundObject() {
	MoveCameraArroundObject* alien = new MoveCameraArroundObject();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->speed);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->path);
	return alien;
} 
