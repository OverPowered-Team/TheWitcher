#include "Trigger_To_Lvl1_Tutorial.h"
#include "UI_DamageCount.h"
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
		GameObject::FindWithName("HUD_Game")->GetChild("UI_InGame")->GetChild("InGame")->GetComponent<UI_DamageCount>()->AddRemainingComboPoints();
		SceneManager::LoadScene("TutorialToLvl1", FadeToBlackType::FADE);
	}
}
