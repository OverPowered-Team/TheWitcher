#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ExplodeChildren;
class CameraMovement;
class PlayerController;

class ALIEN_ENGINE_API BarrelDrop : public Alien {

public:

	BarrelDrop();
	virtual ~BarrelDrop();
	
	void Start();
	void Update();
	void DropPickUp();
	bool RandomDrop(const int min, const int max);
public:
	ExplodeChildren* barrel = nullptr;
	CameraMovement* camera = nullptr;
	PlayerController* p1 = nullptr;
	PlayerController* p2 = nullptr;
};

ALIEN_FACTORY BarrelDrop* CreateBarrelDrop() {
	BarrelDrop* alien = new BarrelDrop();
	// To show in inspector here

	return alien;
} 
