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
			SceneManager::LoadScene("boss_test");
		}

		if (strcmp(current_scene.c_str(), "Level_Mahakam") == 0)
		{
			Scores_Data::player1_kills = GameObject::FindWithName("GameManager")->GetComponent<GameManager>()->player_manager->players[0]->player_data.type_kills;
			Scores_Data::player2_kills = GameObject::FindWithName("GameManager")->GetComponent<GameManager>()->player_manager->players[1]->player_data.type_kills;
		}
		else
		{
			/*Scores_Data::player1_kills += GameObject::FindWithName("GameManager")->GetComponent<GameManager>()->player_manager->players[0]->player_data.type_kills;
			Scores_Data::player2_kills += GameObject::FindWithName("GameManager")->GetComponent<GameManager>()->player_manager->players[1]->player_data.type_kills;*/
		}

		Scores_Data::player1_relics = GameObject::FindWithName("GameManager")->GetComponent<GameManager>()->player_manager->players[0]->relics;
		Scores_Data::player2_relics = GameObject::FindWithName("GameManager")->GetComponent<GameManager>()->player_manager->players[1]->relics;

		Scores_Data::last_scene = SceneManager::GetCurrentScene();

		//if (strcmp(current_scene.c_str(), "Lvl_1") == 0)
		//{
		//	Scores_Data::won_level1 = true;
		//	SceneManager::LoadScene("NewWin_Menu", FadeToBlackType::FADE);
		//}
		//else if (strcmp(current_scene.c_str(), "Level_Mahakam") == 0)
		//{
		//	SceneManager::LoadScene("Wagonnetes");
		//}
		//else 
		if (strcmp(current_scene.c_str(), "Wagonnetes") == 0)
		{
			SceneManager::LoadScene("boss_test");
		}
		else
		{
			Scores_Data::won_level2 = true;
			SceneManager::LoadScene("NewWin_Menu", FadeToBlackType::FADE);
		}
	}
	
}
