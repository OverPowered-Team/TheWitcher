#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class EventManager;
class ComponentAudioEmitter; 
class ComponentText; 

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
	const char* subtitlesText = "None";
	const char* priority = "None";
	bool pauseContinue = true;
	bool paused = false;
	const char* entityName = "noName"; //Author
	const char* subtitles = "";
};

// TODO: Delete this when we know when an audio finishes:
struct SubtitlesTime
{
	float currentTime = 0.0f; 
	float totalTime = 5.0f; 
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
	ComponentText* text = nullptr; 

	// TODO
	SubtitlesTime subtitlesTime; 
};

ALIEN_FACTORY DialogueManager* CreateDialogueManager() {
	DialogueManager* alien = new DialogueManager();
	// To show in inspector here

	return alien;
}

