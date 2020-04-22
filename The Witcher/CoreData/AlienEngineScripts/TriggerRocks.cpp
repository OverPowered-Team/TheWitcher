#include "TriggerRocks.h"
#include "RockSpawner.h"

TriggerRocks::TriggerRocks() : Alien()
{
}

TriggerRocks::~TriggerRocks()
{
}

void TriggerRocks::Start()
{
}

void TriggerRocks::Update()
{
}

void TriggerRocks::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0) {
		game_object->parent->GetComponent<RockSpawner>()->Disable();
	}
}
