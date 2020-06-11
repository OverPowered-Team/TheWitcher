#include "Credits.h"
#include "Scores_Data.h"

Credits::Credits() : Alien()
{
}

Credits::~Credits()
{
}

void Credits::Start()
{
	points1 = game_object->GetChild("Player1_Points")->GetComponent<ComponentText>();
	points2 = game_object->GetChild("Player2_Points")->GetComponent<ComponentText>();

	points1->SetText(std::to_string(Scores_Data::total_player1_points).c_str());
	points2->SetText(std::to_string(Scores_Data::total_player2_points).c_str());
}

void Credits::Update()
{
	if (transform->GetLocalPosition().y < last_position)
	{
		float position_to_add = (last_position / time_credits_last) * Time::GetDT();
		transform->AddPosition(float3(0, position_to_add, 0));
		if (transform->GetLocalPosition().y >= last_position)
		{
			time = Time::GetGameTime();
		}
	}
	else
	{
		if ((Time::GetGameTime() - time) >= 2.f)
		{
			SceneManager::LoadScene("Main_Menu", FadeToBlackType::VERTICAL_CURTAIN);
		}
	}
}