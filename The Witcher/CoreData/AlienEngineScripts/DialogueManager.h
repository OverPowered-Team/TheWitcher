#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"


class ALIEN_ENGINE_API DialogueManager : public Alien {

public:
	struct Dialogue
	{
		const char* audioName = "noName"; 
		unsigned int priority = 1; 
		bool pauseContinue = true; 
		bool paused = false; 
		const char* entityName = "noName"; 
	};

	DialogueManager();
	virtual ~DialogueManager();
	void Start();
	void Update();

	bool InputNewDialogue(const char* audioName, unsigned int priority, bool pauseContinue = true, const char* entityName = "noName") const;

private:
	Dialogue* currentDialogue = nullptr; 
	Dialogue* pausedDialogue = nullptr; 

};

ALIEN_FACTORY DialogueManager* CreateDialogueManager() {
	DialogueManager* alien = new DialogueManager();
	// To show in inspector here

	return alien;
}

