#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API CameraWagonette : public Alien {

public:

	CameraWagonette();
	virtual ~CameraWagonette();
	
	void Start();
	void Update();

private:
	float3 offset;
	GameObject* minecart = nullptr;
};

ALIEN_FACTORY CameraWagonette* CreateCameraWagonette() {
	CameraWagonette* alien = new CameraWagonette();
	// To show in inspector here

	return alien;
} 
