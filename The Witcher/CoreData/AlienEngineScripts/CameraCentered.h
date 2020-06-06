#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API CameraCentered : public Alien {

public:

	CameraCentered();
	virtual ~CameraCentered();

	std::vector<GameObject*> players;
	uint num_curr_players = 0u;

	void Start();
	void Update();

	float distance = 10.f;
	float hor_angle = 0.f;
	float vert_angle = 35.f;
};

ALIEN_FACTORY CameraCentered* CreateCameraCentered() {
	CameraCentered* alien = new CameraCentered();

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->distance);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->hor_angle);
	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->vert_angle);

	return alien;
} 
