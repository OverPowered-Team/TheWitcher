#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include <tuple>
#include <map>

class PlayerController;
struct Dialogue;
class DialogueManager;

class ALIEN_ENGINE_API EventManager : public Alien {

public:
	EventManager();
	virtual ~EventManager();
	void Start();
	void Update();

	void OnPlayerDead(PlayerController* player_dead);
	void OnPlayerRevive(PlayerController* player_revived);

	// Audio name in the dialogue object's emitter bank, priority (use event map), can it be stopped and then continued?, entity name, delay to invoke the audio
	void ReceiveDialogueEvent(Dialogue& dialogue, float delay = 0.f) const;
	void ReceiveDialogueEvent(int index, float volume = 0.5f) const;

	
	

public:
	std::map<const char*, uint> eventPriorities; // event and priority
};

ALIEN_FACTORY EventManager* CreateEventManager() {
	EventManager* alien = new EventManager();
	// To show in inspector here

	return alien;
}