#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API ListenerButton : public Alien {

public:

	ListenerButton();
	virtual ~ListenerButton();
	
	void Start();
	void Update();

	void Exit();


};

ALIEN_FACTORY ListenerButton* CreateListenerButton() {
	ListenerButton* alien = new ListenerButton();
	// To show in inspector here

	return alien;
} 
