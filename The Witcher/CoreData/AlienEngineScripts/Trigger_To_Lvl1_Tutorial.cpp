#include "Trigger_To_Lvl1_Tutorial.h"
#include "UI_DamageCount.h"
#include "GameManager.h"
#include "Scores_Data.h"

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
		Scores_Data::last_checkpoint_position = float3::inf();
		GameManager::instance->PrepareDataNextScene(false);
		SceneManager::LoadScene("TutorialToLvl1", FadeToBlackType::FADE);
	}
}
