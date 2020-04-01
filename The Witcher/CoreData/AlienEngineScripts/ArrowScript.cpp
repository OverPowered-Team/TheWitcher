#include "ArrowScript.h"
#include "PlayerAttacks.h"

ArrowScript::ArrowScript() : Alien()
{
}

ArrowScript::~ArrowScript()
{
}

void ArrowScript::Start()
{
	current_time = Time::GetGameTime();
}

void ArrowScript::Update()
{
	if ((Time::GetGameTime() - current_time) > life_time)
		GameObject::Destroy(game_object->parent);
}

void ArrowScript::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "PlayerAttack") == 0) {
	}
}
