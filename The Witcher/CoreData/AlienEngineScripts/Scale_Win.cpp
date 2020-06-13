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
	damage_done = GameObject::FindWithName("DamageDone")->GetComponent<ComponentText>();

	// Win/Lose
	if (!Scores_Data::dead)
	{
		GameObject::FindWithName("Victory")->SetEnable(true);
		GameObject::FindWithName("Defeat")->SetEnable(false);
		if (Scores_Data::won_level2)
		{
			GameObject::FindWithName("Victory")->GetChild("Text")->GetComponent<ComponentText>()->SetText("Congratulations!");
		}
	}
	else
	{
		GameObject::FindWithName("Victory")->SetEnable(false);
		GameObject::FindWithName("Defeat")->SetEnable(true);
	}

	first_frame = true;
	spawned_invoke = false;
	start_time_damage = Time::GetGameTime();
}

void Scale_Win::Update()
{
	if (is_damage_setting)
	{
		float t = (Time::GetGameTime() - start_time_damage) / damage_time;
		float lerp1 = Maths::Lerp(0, Scores_Data::player1_damage, t);
		float lerp2 = Maths::Lerp(0, Scores_Data::player2_damage, t);

		score_text_1->SetText(std::to_string((int)lerp1).c_str());
		score_text_2->SetText(std::to_string((int)lerp2).c_str());

		if (t >= 0.8f)
		{
			float text_t = (t - 0.8f) / 0.2f;
			float alpha = Maths::Lerp(1.0f, 0.0f, text_t);

			damage_done->SetAlpha(alpha);

			if (text_t >= 1)
			{
				damage_done->SetAlpha(0);
			}
		}

		if (t >= 1)
		{
			score_text_1->SetText(std::to_string(Scores_Data::player1_damage).c_str());
			score_text_2->SetText(std::to_string(Scores_Data::player2_damage).c_str());
			player1_points = Scores_Data::player1_damage;
			player2_points = Scores_Data::player2_damage;
			is_damage_setting = false;
		}

		return;
	}

	if (!spawned_invoke && !is_damage_setting) 
	{
		std::vector<float2> spawn_points;

		spawn_points.reserve(3);
		spawn_points.emplace_back(-2.f, 1.5f);
		spawn_points.emplace_back(2.f, 1.5f);
		spawn_points.emplace_back(0.f, -2.f);

		// Head Spawns
		SpawnHeads(ConvertKillsMapToVector(Scores_Data::player1_kills), spawn_points, 1);
		SpawnHeads(ConvertKillsMapToVector(Scores_Data::player2_kills), spawn_points, 2);

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

void Scale_Win::SpawnHeads(const std::vector<int>& kills, const std::vector<float2>& spawn_points, int player)
{
	if (kills.size() > 0)
	{
		Spawner* spawner = nullptr;
		if (player == 1)
		{
			spawner = spawner_l;
		}
		else
		{
			spawner = spawner_r;
		}

		for (int i = 0; i < kills.size(); ++i)
		{
			float random_time = Random::GetRandomFloatBetweenTwo(0.35f, 0.6f);
			int random_spawn = Random::GetRandomIntBetweenTwo(1, 3);

			Invoke([this, spawn_points, random_spawn, i, spawner, kills]() -> void
				{
					spawner->Spawn(TO_SPAWN::HEAD, float3(spawner->transform->GetGlobalPosition().x + spawn_points[random_spawn - 1].x,
						spawner->transform->GetGlobalPosition().y,
						spawner->transform->GetGlobalPosition().z + spawn_points[random_spawn - 1].y), kills[i]);
				}
				,
					random_time * i);
		}
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
	Scores_Data::player1_damage = 0;
	Scores_Data::player2_damage = 0;
	Scores_Data::player1_kills.clear();
	Scores_Data::player2_kills.clear();

	if (Scores_Data::dead)
	{
		Scores_Data::dead = false;
		if (strcmp(Scores_Data::last_scene.c_str(), "Lvl_1_Tutorial") == 0) {
			SceneManager::LoadScene("ForceLoadTutorial", FadeToBlackType::FADE);
		}
		else if (strcmp(Scores_Data::last_scene.c_str(), "Lvl_1") == 0) {
			SceneManager::LoadScene("TutorialToLvl1", FadeToBlackType::FADE);
		}
		else if (strcmp(Scores_Data::last_scene.c_str(), "Mahakam_1") == 0) {
			SceneManager::LoadScene("BalanToLvl2", FadeToBlackType::FADE);
		}
		else if (strcmp(Scores_Data::last_scene.c_str(), "Wagonnetes") == 0) {
			SceneManager::LoadScene("Lvl2ToWagon", FadeToBlackType::FADE);
		}
		else if (strcmp(Scores_Data::last_scene.c_str(), "boss_test") == 0) {
			SceneManager::LoadScene("WagonToCiri", FadeToBlackType::FADE);
		}
	}
	else
	{
		Scores_Data::last_checkpoint_position = float3::inf();

		Scores_Data::total_player1_points += stoi(score_text_1->GetText());
		Scores_Data::total_player2_points += stoi(score_text_2->GetText());

		if (Scores_Data::won_level1)
		{
			if (Scores_Data::won_level2)
			{
				Scores_Data::won_level1 = false;
				Scores_Data::won_level2 = false;
				Scores_Data::player1_relics.clear();
				Scores_Data::player2_relics.clear();
				SceneManager::LoadScene("CreditsMenu", FadeToBlackType::VERTICAL_CURTAIN);
			}
			else
			{
				SceneManager::LoadScene("BalanToLvl2", FadeToBlackType::VERTICAL_CURTAIN);
			}
		}
	}
}

std::vector<int> Scale_Win::ConvertKillsMapToVector(const std::map<uint, uint>& kills_map)
{
	std::vector<int> vector;

	auto iter = kills_map.begin();
	for (; iter != kills_map.end(); ++iter)
	{
		if ((*iter).first != 4 && (*iter).first != 2 && (*iter).first != 3)
		{
			for (int i = 0; i < (*iter).second; ++i)
			{
				vector.push_back((*iter).first);
			}
		}
	}

	return vector;
}

