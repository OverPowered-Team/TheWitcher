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
	ComponentAudioEmitter* em = collider->game_object_attached->GetComponent<ComponentAudioEmitter>();
	if (em != nullptr && !first)
	{
		SceneManager::LoadScene("Win");
		first = true;
	}
}
