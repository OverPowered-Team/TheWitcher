#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class EventManager;
class ComponentCollider;
class DialogueManager; 

class ALIEN_ENGINE_API DialogueTriggerSimple : public Alien {

public:
	DialogueTriggerSimple();
	virtual ~DialogueTriggerSimple();
	void Start();
	void OnTriggerEnter(ComponentCollider* collider); 

private:
	EventManager* eventManager = nullptr;
};

ALIEN_FACTORY DialogueTriggerSimple* CreateDialogueTriggerSimple() {
	DialogueTriggerSimple* alien = new DialogueTriggerSimple();
	// To show in inspector here

	std::string dialogueName = "Hit_Sword"; // TODO, remaining
	SHOW_IN_INSPECTOR_AS_STRING(dialogueName); 

	return alien;
}


