#include "Scale_Win.h"
#include "Spawner.h"
#include "PlayerManager.h"
#include "PlayerController.h"
#include "Scores_Data.h"

Scale_Win::Scale_Win() : Alien()
{
}

Scale_Win::~Scale_Win()
{
}

void Scale_Win::Start()
{
	// Scales
	left_scale = game_object->GetChild("Left_Scale");
	right_scale = game_object->GetChild("Right_Scale");
	rigid_body1 = left_scale->GetComponent<ComponentRigidBody>();
	rigid_body2 = right_scale->GetComponent<ComponentRigidBody>();
	connector = game_object->GetChild("Connector");

	// Spawners
	spawner_l = GameObject::FindWithName("Left_Spawner")->GetComponent<Spawner>();
	spawner_r = GameObject::FindWithName("Right_Spawner")->GetComponent<Spawner>();

	// Texts
	score_text_1 = GameObject::FindWithName("Score_Player_1")->GetComponent<ComponentText>();
	score_text_2 = GameObject::FindWithName("Score_Player_2")->GetComponent<ComponentText>();
	score_text_1->SetText("0");
	score_text_2->SetText("0");

	// Win/Lose
	if (!Scores_Data::dead)
	{
		GameObject::FindWithName("Victory")->SetEnable(true);
		GameObject::FindWithName("Defeat")->SetEnable(false);
		if (Scores_Data::won_level2)
		{
			GameObject::FindWithName("Victory")->GetChild("Text")->GetComponent<ComponentText>()->SetText("Congratulations! Main Menu");
		}
	}
	else
	{
		GameObject::FindWithName("Victory")->SetEnable(false);
		GameObject::FindWithName("Defeat")->SetEnable(true);
	}

	// Head Spawns
	for (int i = 1; i <= Scores_Data::player1_kills; ++i)
	{
		float random_time = Random::GetRandomFloatBetweenTwo(0.25f, 0.5f);
		float random_pos_x = Random::GetRandomFloatBetweenTwo(-3.25f, 3.25f);
		float random_pos_z = Random::GetRandomFloatBetweenTwo(-3.25f, 3.25f);

		Invoke([this, random_pos_x, random_pos_z]() -> void
			{
				spawner_l->Spawn(TO_SPAWN::HEAD, float3(spawner_l->transform->GetGlobalPosition().x + random_pos_x,
					spawner_l->transform->GetGlobalPosition().y,
					spawner_l->transform->GetGlobalPosition().z + random_pos_z));
			}
			,
				random_time* i);
	}

	for (int i = 1; i <= Scores_Data::player2_kills; ++i)
	{
		float random_time = Random::GetRandomFloatBetweenTwo(0.25f, 0.5f);
		float random_pos_x = Random::GetRandomFloatBetweenTwo(-3.25f, 3.25f);
		float random_pos_z = Random::GetRandomFloatBetweenTwo(-3.25f, 3.25f);

		Invoke([this, random_pos_x, random_pos_z]() -> void
			{
				spawner_r->Spawn(TO_SPAWN::HEAD, float3(spawner_r->transform->GetGlobalPosition().x + random_pos_x,
					spawner_r->transform->GetGlobalPosition().y,
					spawner_r->transform->GetGlobalPosition().z + random_pos_z));
			}
			,
				random_time * i);
	}
}

void Scale_Win::Update()
{
	LerpingText();

	if (!in_place)
	{
		Scale();
	}

	if (Input::GetControllerButtonDown(1, Input::CONTROLLER_BUTTON_Y))
	{
		HandleSceneLoad();
	}
}

void Scale_Win::CalculateInclination()
{
	float ratio = 0.0f;

	if (Maths::Max(player1_points, player2_points) == 0)
	{
		ratio = 1;
	}
	else 
	{
		ratio = (float)Maths::Min(player1_points, player2_points) / (float)Maths::Max(player1_points, player2_points);
	}

	float value = 0.0f;

	if (ratio <= 0.5f)
	{
		value = 1;
	}
	else if (ratio == 1)
	{
		value = 0;
	}
	else
	{
		value = 1 - ((ratio - 0.5f) * 2);
	}

	original_position1 = left_scale->transform->GetLocalPosition().y;
	original_position2 = right_scale->transform->GetLocalPosition().y;

	if (Maths::Min(player1_points, player2_points) == player1_points)
	{
		desired_position1 = max_Y * value;
		desired_position2 = -max_Y * value;
	}
	else
	{
		desired_position1 = -max_Y * value;
		desired_position2 = max_Y * value;
	}

	in_place = false;
	time = Time::GetGameTime();
}

void Scale_Win::LerpingText()
{
	if (player1_points != current_points1)
	{
		int points1 = Maths::Lerp(current_points1, player1_points, (Time::GetGameTime() - time) / time_to_scale);
		if (((Time::GetGameTime() - time) / time_to_scale) >= 1)
		{
			points1 = player1_points;
			current_points1 = player1_points;
		}
		score_text_1->SetText(std::to_string(points1).c_str());
	}

	if (player2_points != current_points2)
	{
		int points2 = Maths::Lerp(current_points2, player2_points, (Time::GetGameTime() - time) / time_to_scale);
		if (((Time::GetGameTime() - time) / time_to_scale) >= 1)
		{
			points2 = player2_points;
			current_points2 = player2_points;
		}
		score_text_2->SetText(std::to_string(points2).c_str());
	}
}

void Scale_Win::Scale()
{
	left_scale->transform->SetGlobalPosition(float3(7.5f, Maths::Lerp(original_position1, desired_position1, (Time::GetGameTime() - time) / time_to_scale), 0));
	right_scale->transform->SetGlobalPosition(float3(-7.5f, Maths::Lerp(original_position2, desired_position2, (Time::GetGameTime() - time) / time_to_scale), 0));

	// Delete this when physics updated rigid body position with GO
	rigid_body1->SetPosition(left_scale->transform->GetLocalPosition());
	rigid_body2->SetPosition(right_scale->transform->GetLocalPosition());
	// ------------------------------------------

	// Connector between plates
	//float3 vector = (left_scale->transform->GetGlobalPosition() - right_scale->transform->GetGlobalPosition()).Normalized();
	//Quat quat = Quat::RotateAxisAngle(float3(0, 0, 1), vector.AngleBetweenNorm(connector->transform->right));
	//connector->transform->SetLocalRotation(quat);

	if (Time::GetGameTime() > time + time_to_scale)
	{
		in_place = true;
	}
}

void Scale_Win::HandleSceneLoad()
{
	if (Scores_Data::dead)
	{
		Scores_Data::dead = false;
		if (Scores_Data::won_level1)
		{
			SceneManager::LoadScene("Mahakam", FadeToBlackType::VERTICAL_CURTAIN);
		}
		else
		{
			SceneManager::LoadScene("Lvl_1_Art_Colliders", FadeToBlackType::VERTICAL_CURTAIN);
		}
	}
	else
	{
		Scores_Data::last_checkpoint_position = float3::inf();
		if (Scores_Data::won_level1)
		{
			SceneManager::LoadScene("Mahakam", FadeToBlackType::VERTICAL_CURTAIN);
		}
		else if (Scores_Data::won_level2)
		{
			Scores_Data::won_level1 = false;
			Scores_Data::won_level2 = false;
			SceneManager::LoadScene("EndGame_Menu", FadeToBlackType::VERTICAL_CURTAIN);
		}
	}
}

