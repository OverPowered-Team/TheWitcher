#include "ArrowScript.h"
#include "PlayerController.h"

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
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0) {
		static_cast<PlayerController*>(collider->game_object_attached->GetComponentScript("PlayerController"))->ReceiveDamage(damage);
		GameObject::Destroy(game_object->parent);
	}
}
