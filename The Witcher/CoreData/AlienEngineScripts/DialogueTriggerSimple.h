#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class EventManager;
class ComponentCollider;

class ALIEN_ENGINE_API DialogueTriggerSimple : public Alien {

public:
	DialogueTriggerSimple();
	virtual ~DialogueTriggerSimple();
	void OnTriggerEnter(ComponentCollider* collider);

public:
	bool once = true;
	bool hitToPlay = false;
	int diaLogueIndex = 0;
	float volume = 0.5f;
	bool specificEntity = false;
	bool Geralt = false;   

};

ALIEN_FACTORY DialogueTriggerSimple* CreateDialogueTriggerSimple() {
	DialogueTriggerSimple* alien = new DialogueTriggerSimple();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->once);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->specificEntity);
	SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(alien->Geralt);
	SHOW_IN_INSPECTOR_AS_INPUT_INT(alien->diaLogueIndex);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->volume);

	return alien;
}



