#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API ScaleImage : public Alien {

public:

	ScaleImage();
	virtual ~ScaleImage();
	
	void Start();
	void Update();
	float3 scale = float3::zero();
	float duration = 0;
	float start_time = 0;
	float3 start_scale = float3::zero();
	bool first_start = true;
};

ALIEN_FACTORY ScaleImage* CreateScaleImage() {
	ScaleImage* alien = new ScaleImage();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->scale.x);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->scale.y);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->scale.z);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->duration);

	return alien;
} 
