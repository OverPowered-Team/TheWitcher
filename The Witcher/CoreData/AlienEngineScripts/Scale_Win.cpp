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
	start_position1 = right_scale->transform->GetLocalPosition().y;
	start_position2 = left_scale->transform->GetLocalPosition().y;
	
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

	first_frame = true;
	spawned_invoke = false;
}

void Scale_Win::Update()
{
	if (!first_frame && !spawned_invoke) {
		std::vector<float2> spawn_points;

		spawn_points.reserve(3);
		spawn_points.emplace_back(float2(-2.f, 1.5f));
		spawn_points.emplace_back(float2(2.f, 1.5f));
		spawn_points.emplace_back(float2(0.f, -2.f));

		// Head Spawns
		if (Scores_Data::player1_kills > 0)
		{
			for (int i = 1; i <= Scores_Data::player1_kills; ++i)
			{
				float random_time = Random::GetRandomFloatBetweenTwo(0.35f, 0.6f);
				int random_spawn = Random::GetRandomIntBetweenTwo(1, 3);

				Invoke([this, spawn_points, random_spawn]() -> void
					{
						spawner_l->Spawn(TO_SPAWN::HEAD, float3(spawner_l->transform->GetGlobalPosition().x + spawn_points[random_spawn - 1].x,
							spawner_l->transform->GetGlobalPosition().y,
							spawner_l->transform->GetGlobalPosition().z + spawn_points[random_spawn - 1].y));
					}
					,
						random_time * i);
			}
		}

		if (Scores_Data::player2_kills > 0)
		{
			for (int i = 1; i <= Scores_Data::player2_kills; ++i)
			{
				float random_time = Random::GetRandomFloatBetweenTwo(0.35f, 0.6f);
				int random_spawn = Random::GetRandomIntBetweenTwo(1, 3);

				Invoke([this, spawn_points, random_spawn]() -> void
					{
						spawner_r->Spawn(TO_SPAWN::HEAD, float3(spawner_r->transform->GetGlobalPosition().x + spawn_points[random_spawn - 1].x,
							spawner_r->transform->GetGlobalPosition().y,
							spawner_r->transform->GetGlobalPosition().z + spawn_points[random_spawn - 1].y));
					}
					,
						random_time * i);
			}
		}
		spawned_invoke = true;
	}

	LerpingText();

	if (!in_place)
	{
		Scale();
	}

	if (Input::GetControllerButtonDown(1, Input::CONTROLLER_BUTTON_Y)
		|| Input::GetControllerButtonDown(2, Input::CONTROLLER_BUTTON_Y))
	{
		HandleSceneLoad();
	}

	first_frame = false;
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

	original_position1 = right_scale->transform->GetLocalPosition().y;
	original_position2 = left_scale->transform->GetLocalPosition().y;

	if (Maths::Min(player1_points, player2_points) == player2_points)
	{
		desired_position1 = original_position1 + (max_Y * value);
		desired_position2 = original_position2 - (max_Y * value);
	}
	else
	{
		desired_position1 = original_position1 - (max_Y * value);
		desired_position2 = original_position2 + (max_Y * value);
	}

	if (desired_position1 > (start_position1 + (max_Y * value)))
	{
		desired_position1 = start_position1 + (max_Y * value);
		desired_position2 = start_position2 - (max_Y * value);
	}
	else if (desired_position1 < (start_position1 - (max_Y * value)))
	{
		desired_position1 = start_position1 - (max_Y * value);
		desired_position2 = start_position2 + (max_Y * value);
	}

	if (desired_position2 > (start_position2 + (max_Y * value)))
	{
		desired_position2 = start_position2 + (max_Y * value);
		desired_position1 = start_position1 - (max_Y * value);
	}
	else if (desired_position2 < (start_position2 - (max_Y * value)))
	{
		desired_position2 = start_position2 - (max_Y * value);
		desired_position1 = start_position1 + (max_Y * value);
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
	float3 posR = right_scale->transform->GetGlobalPosition();
	float3 posL = left_scale->transform->GetGlobalPosition();


	right_scale->transform->SetLocalPosition(float3(7.5f, Maths::Lerp(original_position1, desired_position1, (Time::GetGameTime() - time) / time_to_scale), 0));
	left_scale->transform->SetLocalPosition(float3(-7.5f, Maths::Lerp(original_position2, desired_position2, (Time::GetGameTime() - time) / time_to_scale), 0));

	// Delete this when physics updated rigid body position with GO
	rigid_body1->SetPosition(left_scale->transform->GetLocalPosition());
	rigid_body2->SetPosition(right_scale->transform->GetLocalPosition());
	// ------------------------------------------

	// Connector between plates
	float3 vector = right_scale->transform->GetGlobalPosition() - left_scale->transform->GetGlobalPosition();
	bool is_right_up = vector.y >= (posR-posL).y;
	connector->transform->SetGlobalRotation(connector->transform->GetGlobalRotation() * Quat::RotateAxisAngle((is_right_up) ? -float3::unitX() : float3::unitX(), vector.AngleBetween(posR - posL)));

	if (Time::GetGameTime() > time + time_to_scale)
	{
		in_place = true;
	}
}

void Scale_Win::HandleSceneLoad()
{
	//if (Scores_Data::dead)
	//{
	//	Scores_Data::dead = false;
	//	Scores_Data::player1_kills = 0;
	//	Scores_Data::player2_kills = 0;
	//	SceneManager::LoadScene(Scores_Data::last_scene.c_str());
	//}
	//else
	//{
	//	Scores_Data::last_checkpoint_position = float3::inf();
	//	if (Scores_Data::won_level1)
	//	{
	//		if (Scores_Data::won_level2)
	//		{
	//			//Scores_Data::won_level1 = false;
	//			//Scores_Data::won_level2 = false;
	//			SceneManager::LoadScene("CreditsMenu", FadeToBlackType::VERTICAL_CURTAIN);
	//		}
	//		else
	//		{
	//			SceneManager::LoadScene("Wagonnetes", FadeToBlackType::VERTICAL_CURTAIN);
	//		}
	//	}
	//	else {
	//		LOG("WTF are you doing entering here stupid?");
	//	}
	//}
	SceneManager::LoadScene("CreditsMenu", FadeToBlackType::VERTICAL_CURTAIN);
}

