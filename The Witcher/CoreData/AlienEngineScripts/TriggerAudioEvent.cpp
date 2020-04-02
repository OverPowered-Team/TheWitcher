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
	timer = Time::GetGameTime();
}

void TriggerAudioEvent::Update()
{
}

void TriggerAudioEvent::OnTriggerEnter(ComponentCollider* collider)
{
	if (Time::GetGameTime() - timer >= 1.f)
	{
		if (!triggered && strcmp(collider->game_object_attached->GetTag(), "Player") == 0) {
			emitter->StartSound();
			triggered = true;
		}
	}
}

void TriggerAudioEvent::OnTriggerExit(ComponentCollider* collider)
{
}
