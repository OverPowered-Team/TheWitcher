#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Dummie : public Alien {

public:

	Dummie();
	virtual ~Dummie();
	
	void Start();
	void Update();

	void OnTriggerEnter(ComponentCollider* col) override;
};

ALIEN_FACTORY Dummie* CreateDummie() {
	Dummie* alien = new Dummie();
	// To show in inspector here

	return alien;
} 
