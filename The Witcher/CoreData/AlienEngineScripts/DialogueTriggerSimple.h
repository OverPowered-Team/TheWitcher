#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class EventManager;
class ComponentCollider;
class DialogueManager; 
struct Dialogue; 

#include "DialogueManager.h"

class ALIEN_ENGINE_API DialogueTriggerSimple : public Alien {

public:
	DialogueTriggerSimple();
	virtual ~DialogueTriggerSimple();
	void Start();
	void Update(); 
	void OnTriggerEnter(ComponentCollider* collider); 

private: 
	void EmitDialogue(); 
public: 
	bool once = false;
	bool hitToPlay = false; 
	Dialogue dialogue; 
private:
	EventManager* eventManager = nullptr;
	
};

ALIEN_FACTORY DialogueTriggerSimple* CreateDialogueTriggerSimple() {
	DialogueTriggerSimple* alien = new DialogueTriggerSimple();
	// To show in inspector here

	// TODO: with proper names (won't work)
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->once);
	SHOW_IN_INSPECTOR_AS_STRING(alien->dialogue.audioData.eventName);
	SHOW_IN_INSPECTOR_AS_STRING(alien->dialogue.entityName);
	SHOW_IN_INSPECTOR_AS_STRING(alien->dialogue.priority);
	SHOW_IN_INSPECTOR_AS_STRING(alien->dialogue.subtitlesText);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->hitToPlay);

	return alien;
}


