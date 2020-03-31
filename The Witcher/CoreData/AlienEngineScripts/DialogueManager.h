#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class EventManager;
class ComponentAudioEmitter; 

struct AudioData
{
	const char* eventName = "noName";
	const char* groupID = "noName";
	const char* stateID = "noName";
};

struct Dialogue
{
public: 
	AudioData audioData; 
	const char* priority = "None";
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

	bool InputNewDialogue(Dialogue &dialogue);

private: 
	void OverrideDialogue(Dialogue& newDialogue); 

private:
	Dialogue currentDialogue; 
	Dialogue pausedDialogue; 

	EventManager* eventManager = nullptr;
	ComponentAudioEmitter* audioEmitter = nullptr; 
};

ALIEN_FACTORY DialogueManager* CreateDialogueManager() {
	DialogueManager* alien = new DialogueManager();
	// To show in inspector here

	return alien;
}

