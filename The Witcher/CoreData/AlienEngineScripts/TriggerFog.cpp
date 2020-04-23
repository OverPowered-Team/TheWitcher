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
	ComponentCamera* camera = collider->game_object_attached->GetComponent<ComponentCamera>();
	if (camera != nullptr)
	{
		parentScript->RecieveCollisionEnterInteraction(collider_index);
	}
}

void TriggerFog::OnTriggerExit(ComponentCollider* collider)
{
	ComponentCamera* camera = collider->game_object_attached->GetComponent<ComponentCamera>();
	if (camera != nullptr)
	{
		parentScript->RecieveCollisionExitInteraction(collider_index);
	}
}

void TriggerFog::Update()
{
}