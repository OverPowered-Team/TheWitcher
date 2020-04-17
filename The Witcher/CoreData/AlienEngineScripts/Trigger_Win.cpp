#include "Trigger_Win.h"

Trigger_Win::Trigger_Win() : Alien()
{
}

Trigger_Win::~Trigger_Win()
{
}

void Trigger_Win::Start()
{
}

void Trigger_Win::Update()
{
}

void Trigger_Win::OnTriggerEnter(ComponentCollider* collider)
{
	ComponentAudioEmitter* em = (ComponentAudioEmitter*)collider->game_object_attached->GetComponent(ComponentType::A_EMITTER);
	if (Time::GetGameTime() - timer >= 2.f)
	{
		if (em != nullptr && !first)
		{
			SceneManager::LoadScene("Win");
			first = true;
		}
	}
	
}
