#include "TriggerFog.h"
#include "ProgressiveFog.h"

TriggerFog::TriggerFog() : Alien()
{
}

TriggerFog::~TriggerFog()
{
}

void TriggerFog::Start()
{
	parentScript = game_object->GetComponentInParent<ProgressiveFog>();
}

void TriggerFog::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0)
	{
		parentScript->RecieveCollisionEnterInteraction(collider_index, collider->game_object_attached);
	}
}

void TriggerFog::OnTriggerExit(ComponentCollider* collider)
{
	
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0)
	{
		parentScript->RecieveCollisionExitInteraction(collider_index, collider->game_object_attached);
	}
}

void TriggerFog::Update()
{
}