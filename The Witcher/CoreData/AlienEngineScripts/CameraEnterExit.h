#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "TriggerCamera.h"

class ALIEN_ENGINE_API CameraEnterExit : public Alien {

public:
	enum (CameraTriggerType,
		ENTER,
		EXIT);

	CameraEnterExit();
	virtual ~CameraEnterExit();
	
	void Start();
	void Update();

	void OnTriggerEnter(ComponentCollider* collider) override;
	void OnTriggerExit(ComponentCollider* collider) override;

	CameraTriggerType type = CameraTriggerType::ENTER;

	TriggerCamera* script_parent = nullptr;
};

ALIEN_FACTORY CameraEnterExit* CreateCameraEnterExit() {
	CameraEnterExit* alien = new CameraEnterExit();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(CameraEnterExit::CameraTriggerType, alien->type);

	return alien;
} 
