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
	ComponentAudioEmitter* em = collider->game_object_attached->GetComponent<ComponentAudioEmitter>();
	if (Time::GetGameTime() - timer >= 2.f)
	{
		if (em != nullptr && !first)
		{
			Scores_Data::player1_kills = GameObject::FindWithName("GameManager")->GetComponent<GameManager>()->player_manager->players[0]->player_data.total_kills;
			Scores_Data::player2_kills = GameObject::FindWithName("GameManager")->GetComponent<GameManager>()->player_manager->players[1]->player_data.total_kills;
			Scores_Data::won_level1 = true;
			SceneManager::LoadScene("NewWin_Menu", FadeToBlackType::FADE);
			first = true;
		}
	}
	
}
