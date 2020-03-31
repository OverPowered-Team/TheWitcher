#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class TriggerCamera;

class ALIEN_ENGINE_API CameraEnterExit : public Alien {

public:
	CameraEnterExit();
	virtual ~CameraEnterExit();
	
	void Start();

	void OnTriggerEnter(ComponentCollider* collider) override;

	void OnDrawGizmos() override;

	int collider_position = 0;//The position of the collider, can be 0 for first and 1 for second

	TriggerCamera* script_parent = nullptr;
};

ALIEN_FACTORY CameraEnterExit* CreateCameraEnterExit() {
	CameraEnterExit* alien = new CameraEnterExit();

	SHOW_IN_INSPECTOR_AS_INPUT_INT(alien->collider_position);

	return alien;
} 
