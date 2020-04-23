#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API TriggerAudioEvent : public Alien {
public:
	TriggerAudioEvent();
	virtual ~TriggerAudioEvent();
	void Start();
	void Update();
	void OnTriggerEnter(ComponentCollider* collider);
	void OnTriggerExit(ComponentCollider* collider);

public:
	ComponentAudioEmitter* emitter = nullptr;
	bool triggered = false;
};

ALIEN_FACTORY TriggerAudioEvent* CreateTriggerAudioEvent() {
	TriggerAudioEvent* alien = new TriggerAudioEvent();
	// To show in inspector here
	//SHOW_IN_INSPECTOR_AS_STRING(alien->event_name);

	return alien;
}

