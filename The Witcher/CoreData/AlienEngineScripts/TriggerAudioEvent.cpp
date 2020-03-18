#include "TriggerAudioEvent.h"

TriggerAudioEvent::TriggerAudioEvent(): Alien()
{
}

TriggerAudioEvent::~TriggerAudioEvent()
{
}

void TriggerAudioEvent::Start()
{
	emitter = (ComponentAudioEmitter*)this->game_object->GetComponent(ComponentType::A_EMITTER);
}

void TriggerAudioEvent::Update()
{
}

void TriggerAudioEvent::OnTriggerEnter(ComponentCollider* collider)
{
	if (!triggered) {
		emitter->StartSound();
		triggered = true;
	}
}

void TriggerAudioEvent::OnTriggerExit(ComponentCollider* collider)
{
}
