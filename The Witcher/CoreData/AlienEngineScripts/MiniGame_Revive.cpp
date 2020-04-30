#include "GameManager.h"
#include "PlayerManager.h"
#include "PlayerController.h"
#include "MiniGame_Revive.h"

MiniGame_Revive::MiniGame_Revive() : Alien()
{
}

MiniGame_Revive::~MiniGame_Revive()
{
}

void MiniGame_Revive::Start()
{
	minigame = game_object->GetChild("Minigame");
	moving_part = game_object->GetChild("Minigame")->GetChild("Moving");
	good_part = game_object->GetChild("Minigame")->GetChild("Good_Part");
	start_X = game_object->GetChild("Start_X");
	game_A = game_object->GetChild("Minigame")->GetChild("Y");
	good_part->transform->SetLocalScale(original_scale_green, good_part->transform->GetLocalScale().y, good_part->transform->GetLocalScale().z);
	start_X->SetEnable(true);
	minigame->SetEnable(false);

	revive_state = States::PREGAME;
}

void MiniGame_Revive::Update()
{
	switch (revive_state)
	{
	case States::PREGAME: 
	{
		auto i = GameManager::instance->player_manager->players.begin();
		for (; i != GameManager::instance->player_manager->players.end(); ++i)
		{
			if (player_dead != (*i))
			{
				if ((*i)->CheckForPossibleRevive())
				{
					start_X->SetEnable(true);
				}
				else
				{
					start_X->SetEnable(false);
				}
			}
		}

		break;
	}
	case States::MINIGAME:
	{
		if (actual_inputs == input_times)
		{
			minigame->SetEnable(false);

			revive_state = States::POSTGAME;
		}
		else
		{
			Minigame();

			if (effects_change)
			{
				LerpsOnInput();
			}
		}
		
		break;
	}
	case States::POSTGAME:
	{
		player_dead->Revive(revive_percentatge);
		player_reviving->SetState(StateType::IDLE);
		EndMinigame();
		break;
	}
	}
}

void MiniGame_Revive::Minigame()
{
	float position_x = Maths::Lerp(-1.0f * sign, 1.0f * sign, (Time::GetGameTime() - time) / lerp_time);

	moving_part->transform->SetLocalPosition(float3(position_x, 0, 0));

	if ((Input::GetKeyDown(SDL_SCANCODE_SPACE) || Input::GetControllerButtonDown(player_reviving->controller_index, Input::CONTROLLER_BUTTON_A)) && !effects_change)
	{
		Effects();

		float points = 0.0f;

		if ((good_part->transform->GetLocalScale().x) >= Maths::Abs(position_x))
		{
			++correct_inputs;
			points = 1.f / input_times;
			previous_scale = good_part->transform->GetLocalScale().x;
			new_scale = original_scale_green - ((original_scale_green - desired_scale_green) / (input_times - 1)) * correct_inputs;
			green_reducing = true;
		}
		else
		{
			points = (1 - Maths::Abs(position_x)) / input_times;
			green_reducing = false;
		}

		++actual_inputs;
		revive_percentatge += points;
	}

	if (effects_change && (color_time + 0.1f < Time::GetGameTime()))
	{
		moving_part->GetComponent<ComponentImage>()->SetBackgroundColor(1, 1, 1, 1);
		effects_change = false;
	}

	if (lerp_time + time < Time::GetGameTime())
	{
		sign = -sign;
		time = Time::GetGameTime();
	}
}

void MiniGame_Revive::Effects()
{
	effects_change = true;

	// Bar Color
	moving_part->GetComponent<ComponentImage>()->SetBackgroundColor(1, 1, 0, 1);
	color_time = Time::GetGameTime();
}

void MiniGame_Revive::LerpsOnInput()
{
	// Button Lerp
	if (color_time + 0.05f > Time::GetGameTime())
	{
		float lerp = ((Time::GetGameTime() - color_time) * 20);

		// Button Lerp
		float scale_x = Maths::Lerp(0.5f, 0.75f, lerp);
		float scale_y = Maths::Lerp(3.167f, 4.75f, lerp);
		game_A->transform->SetLocalScale(scale_x, scale_y, 1);

	}
	else if (color_time + 0.05f <= Time::GetGameTime())
	{
		float lerp = (((Time::GetGameTime() - color_time) - 0.05f) * 20);

		// Button Lerp
		float scale_x = Maths::Lerp(0.75, 0.5f, lerp);
		float scale_y = Maths::Lerp(4.75f, 3.167f, lerp);
		game_A->transform->SetLocalScale(scale_x, scale_y, 1);
	}

	if (green_reducing)
	{
		// Green Part Reduction
		float lerp = (Time::GetGameTime() - color_time) * 10;
		float scale = Maths::Lerp(previous_scale, new_scale, lerp);
		good_part->transform->SetLocalScale(scale, good_part->transform->GetLocalScale().y, good_part->transform->GetLocalScale().z);
	}
}

void MiniGame_Revive::StartMinigame(PlayerController* player_reviving)
{
	start_X->SetEnable(false);
	minigame->SetEnable(true);
	this->player_reviving = player_reviving;

	revive_state = States::MINIGAME;
}

void MiniGame_Revive::EndMinigame()
{
	Destroy(this->game_object);
}

void MiniGame_Revive::RestartMinigame()
{
	start_X->SetEnable(true);
	minigame->SetEnable(false);

	revive_percentatge = 0.0f;
	time = 0.0f;
	color_time = 0.0f;
	actual_inputs = 0;
	effects_change = false;
	sign = 1;

	this->player_reviving = nullptr;

	revive_state = States::PREGAME;
}
