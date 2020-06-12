#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class Tutorial_HUD;
class PlayerController;

enum(TRIGGER_TYPE,
	ATTACK,
	DASH,
	MAGIC,
	RELICS,

	ANY = -1,
	);

class ALIEN_ENGINE_API Tutorial_HUD_Triggers : public Alien {

public:

	Tutorial_HUD_Triggers();
	virtual ~Tutorial_HUD_Triggers();
	
public:

	TRIGGER_TYPE trigger_type = TRIGGER_TYPE::ANY;

private:

	void Start();
	void Update();
	void OnTriggerEnter(ComponentCollider* col) override;
	void OnTriggerExit(ComponentCollider* col) override;

private:

	bool first_time = false;
	std::list<PlayerController*> players_triggered_attack;
	std::list<PlayerController*> players_triggered_dash;
	std::list<PlayerController*> players_triggered_magic;
	std::list<PlayerController*> players_triggered_relics;

};

ALIEN_FACTORY Tutorial_HUD_Triggers* CreateTutorial_HUD_Triggers() {
	Tutorial_HUD_Triggers* alien = new Tutorial_HUD_Triggers();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_ENUM(TRIGGER_TYPE, alien->trigger_type);

	return alien;
} 
