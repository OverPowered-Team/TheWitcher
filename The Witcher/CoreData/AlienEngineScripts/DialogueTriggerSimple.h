#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class EventManager;
class ComponentCollider;
class DialogueManager; 
struct Dialogue; 

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
	bool once = true;
	bool hitToPlay = false;

	//Dialogue dialogue;
	int diaLogueIndex = 0;
	float volume = 0.5f;
	bool specificEntity = false;
	bool Geralt = false;  // we need to change this for an enity name (?)

private: 
	EventManager* eventManager = nullptr; 
};

ALIEN_FACTORY DialogueTriggerSimple* CreateDialogueTriggerSimple() {
	DialogueTriggerSimple* alien = new DialogueTriggerSimple();
	// To show in inspector here

	// TODO: with proper names (won't work)
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->once);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->specificEntity);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->Geralt);
	/*SHOW_IN_INSPECTOR_AS_STRING(alien->dialogue.audioData.eventName);
	SHOW_IN_INSPECTOR_AS_STRING(alien->dialogue.entityName);
	SHOW_IN_INSPECTOR_AS_STRING(alien->dialogue.priority);
	SHOW_IN_INSPECTOR_AS_STRING(alien->dialogue.subtitlesText);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->hitToPlay);*/
	SHOW_IN_INSPECTOR_AS_INPUT_INT(alien->diaLogueIndex);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->volume);


	return alien;
}



