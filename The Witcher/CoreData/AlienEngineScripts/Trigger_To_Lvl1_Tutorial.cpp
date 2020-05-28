#include "Trigger_To_Lvl1_Tutorial.h"

Trigger_To_Lvl1_Tutorial::Trigger_To_Lvl1_Tutorial() : Alien()
{
}

Trigger_To_Lvl1_Tutorial::~Trigger_To_Lvl1_Tutorial()
{
}

void Trigger_To_Lvl1_Tutorial::Start()
{
}

void Trigger_To_Lvl1_Tutorial::Update()
{
}

void Trigger_To_Lvl1_Tutorial::OnTriggerEnter(ComponentCollider* col)
{
	if (strcmp(col->game_object_attached->GetTag(), "Player") == 0)
	{
		SceneManager::LoadScene("Lvl_1");
	}
}
