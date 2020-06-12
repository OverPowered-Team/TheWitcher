#include "Trigger_Win.h"
#include "GameManager.h"
#include "PlayerManager.h"
#include "PlayerController.h"
#include "Scores_Data.h"

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
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0)
	{
		std::string current_scene = SceneManager::GetCurrentScene();

		if (strcmp(current_scene.c_str(), "Wagonnetes") == 0)
		{
			SceneManager::LoadScene("WagonToCiri", FadeToBlackType::FADE);
		}
		else if (strcmp(current_scene.c_str(), "Mahakam_1") == 0)
		{
			GameManager::instance->PrepareDataNextScene(false);
			SceneManager::LoadScene("Lvl2ToWagon", FadeToBlackType::FADE);
		}	
		else 
		{
			Scores_Data::won_level2 = true;
			SceneManager::LoadScene("NewWin_Menu", FadeToBlackType::FADE);
		}
	}
}