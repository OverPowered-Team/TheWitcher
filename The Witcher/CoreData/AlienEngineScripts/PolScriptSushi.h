#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API PolScriptSushi : public Alien {

public:

	PolScriptSushi();
	virtual ~PolScriptSushi();
	
	void Start();
	void Update();

	void EatSushi();

};

ALIEN_FACTORY PolScriptSushi* CreatePolScriptSushi() {
	PolScriptSushi* sushi = new PolScriptSushi();
	// To show in inspector here
	//SHOW_VOID_FUNCTION(PolScriptSushi::EatSushi, sushi);
	return sushi;
} 
