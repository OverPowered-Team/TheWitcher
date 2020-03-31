#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class EventManager;

struct Dialogue
{
	const char* audioName = "noName";
	const char* priority;
	bool pauseContinue = true;
	bool paused = false;
	const char* entityName = "noName"; //Author
};

class ALIEN_ENGINE_API DialogueManager : public Alien {

public:


	DialogueManager();
	virtual ~DialogueManager();
	void Start();
	void Update();

	bool InputNewDialogue(Dialogue &dialogue) const;

private:
	Dialogue* currentDialogue = nullptr; 
	Dialogue* pausedDialogue = nullptr; 


	EventManager* eventManager = nullptr;

};

ALIEN_FACTORY DialogueManager* CreateDialogueManager() {
	DialogueManager* alien = new DialogueManager();
	// To show in inspector here

	return alien;
}

