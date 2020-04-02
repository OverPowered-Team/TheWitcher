#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class EventManager;
class ComponentAudioEmitter; 
class ComponentText; 

struct AudioData
{
	std::string eventName = "noName";
	std::string groupID = "noName";
	std::string stateID = "noName";

	void Reset()
	{
		eventName = "None"; 
		groupID = "None";
		stateID = "None";
	}
};

struct SubtitlesTime
{
	float currentTime = 0.0f;
	float totalTime = 5.0f;

	void Reset()
	{
		currentTime = 0.0f; 
		totalTime = 5.0f; 
	}
};

struct Dialogue
{
public: 
	AudioData audioData; 
	SubtitlesTime subtitlesTime;
	std::string subtitlesText = "None";
	std::string  priority = "Default";
	bool pauseContinue = true;
	bool paused = false;
	std::string entityName = "noName"; //Author

public: 
	void Reset()
	{
		audioData.Reset(); 
		subtitlesTime.Reset(); 
		subtitlesText = "None"; 
		priority = "Default"; 
		pauseContinue = true; 
		paused = false; 
		entityName = "noName"; 
	}
};



class ALIEN_ENGINE_API DialogueManager : public Alien {

public:
	DialogueManager();
	virtual ~DialogueManager();
	void Start();
	void Update();

	bool InputNewDialogue(Dialogue &dialogue);
	bool InputNewDialogue(int index);

private: 
	void OverrideDialogue(Dialogue& newDialogue); 
	void LoadJSONDialogues();

private:
	Dialogue currentDialogue; 
	Dialogue pausedDialogue; 

	EventManager* eventManager = nullptr;
	ComponentAudioEmitter* audioEmitter = nullptr; 
	ComponentText* text = nullptr; 

	// to read from JSON
	std::vector <std::tuple<std::string, std::string, float>> dialogueData; // event name, subtitles and subtitle time 

};

ALIEN_FACTORY DialogueManager* CreateDialogueManager() {
	DialogueManager* alien = new DialogueManager();
	// To show in inspector here

	return alien;
}

