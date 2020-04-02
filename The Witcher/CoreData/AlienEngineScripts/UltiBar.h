#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API UltiBar : public Alien {

public:

	UltiBar();
	virtual ~UltiBar();
	
	void Start();
	void Update();

	GameObject* ultibar = nullptr;
	GameObject* ultibar_charged = nullptr;
	
	ComponentBar* normal_ulti = nullptr;
	ComponentBar* charged_ulti = nullptr;

};

ALIEN_FACTORY UltiBar* CreateUltiBar() {
	UltiBar* alien = new UltiBar();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->ultibar);
	SHOW_IN_INSPECTOR_AS_GAMEOBJECT(alien->ultibar_charged);

	return alien;
} 
