#include "Tutorial_HUD_Triggers.h"
#include "Tutorial_HUD.h"

Tutorial_HUD_Triggers::Tutorial_HUD_Triggers() : Alien()
{
}

Tutorial_HUD_Triggers::~Tutorial_HUD_Triggers()
{
}

void Tutorial_HUD_Triggers::Start()
{
}

void Tutorial_HUD_Triggers::Update()
{
}

void Tutorial_HUD_Triggers::OnTriggerEnter(ComponentCollider* col)
{
	switch (trigger_type)
	{
	case TRIGGER_TYPE::ATTACK:
	{
		game_object->parent->parent->GetChild("HUD")->GetComponent<Tutorial_HUD>()->ShowTriggerAttack();
		break;
	}
	case TRIGGER_TYPE::DASH:
		break;
	case TRIGGER_TYPE::ROCKS:
		break;
	case TRIGGER_TYPE::ANY:
		break;
	}
}
