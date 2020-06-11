#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API CiriDeadZone : public Alien {

public:

	CiriDeadZone();
	virtual ~CiriDeadZone();
	
	void Start();
	void Update();

	void OnTriggerEnter(ComponentCollider* collider);

};

ALIEN_FACTORY CiriDeadZone* CreateCiriDeadZone() {
	CiriDeadZone* alien = new CiriDeadZone();
	// To show in inspector here

	return alien;
} 
