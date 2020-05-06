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
		Scores_Data::player1_kills = GameObject::FindWithName("GameManager")->GetComponent<GameManager>()->player_manager->players[0]->player_data.total_kills;
		Scores_Data::player2_kills = GameObject::FindWithName("GameManager")->GetComponent<GameManager>()->player_manager->players[1]->player_data.total_kills;

		if (!Scores_Data::won_level1)
		{
			Scores_Data::won_level1 = true;
		}
		else
		{
			Scores_Data::won_level2 = true;
		}

		SceneManager::LoadScene("NewWin_Menu");
	}
	
}
