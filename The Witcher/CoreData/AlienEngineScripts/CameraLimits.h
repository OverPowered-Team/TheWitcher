#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API CameraLimits : public Alien {

public:

	CameraLimits();
	virtual ~CameraLimits();
	
	void Start();
	void Update();

	void OnDrawGizmos() override;

};

ALIEN_FACTORY CameraLimits* CreateCameraLimits() {
	CameraLimits* alien = new CameraLimits();
	// To show in inspector here

	return alien;
} 
