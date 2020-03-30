#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class PlayerController;

class ALIEN_ENGINE_API EventManager : public Alien {

public:

	EventManager();
	virtual ~EventManager();
	
	void Start();
	void Update();

	GameObject** players_go;
	std::vector<PlayerController*> players;
	uint players_size = 0;

	void OnPlayerDead(PlayerController* player_dead);
	void OnPlayerRevive(PlayerController* player_revived);

    // Audio name in the dialogue object's emitter bank, priority, can it be stopped and then continued?, entity name, delay to invoke the audio
	void ReceiveDialogueEvent(const char* audioName, unsigned int priority, bool PauseContinue = true, const char* entityName = "noName", float delay = 0.f) const;
};

ALIEN_FACTORY EventManager* CreateEventManager() {
	EventManager* alien = new EventManager();
	// To show in inspector here

	return alien;
} 
