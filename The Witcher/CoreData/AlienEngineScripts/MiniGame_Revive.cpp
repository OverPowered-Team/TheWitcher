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
	minigame->SetEnable(false);
	moving_part = game_object->GetChild("Minigame")->GetChild("Moving");
	start_X = game_object->GetChild("Start_X");
	start_X->SetEnable(true);
	game_A = game_object->GetChild("Minigame")->GetChild("Y");
	text = GameObject::FindWithName("Text");
	text->SetEnable(false);
	revive_state = States::PREGAME;
}

void MiniGame_Revive::Update()
{
	switch (revive_state)
	{
	case States::PREGAME: {
	} break;
	case States::MINIGAME:
	{
		if (actual_inputs == input_times)
		{
			minigame->SetEnable(false);
			text->SetEnable(true);
			text->GetComponent<ComponentText>()->SetText(std::to_string(revive_percentatge).c_str());

			revive_state = States::POSTGAME;
		}
		else
		{
			Minigame();

			// Button Lerp
			if (effects_change)
			{
				if (color_time + 0.05f > Time::GetGameTime())
				{
					float lerp = ((Time::GetGameTime() - color_time) * 20);
					float scale_x = Maths::Lerp(0.5f, 0.75f, lerp);
					float scale_y = Maths::Lerp(3.167f, 4.75f, lerp);
					game_A->transform->SetLocalScale(scale_x, scale_y, 1);
				}
				else if (color_time + 0.05f <= Time::GetGameTime())
				{
					float lerp = (((Time::GetGameTime() - color_time) - 0.05f) * 20);
					float scale_x = Maths::Lerp(0.75, 0.5f, lerp);
					float scale_y = Maths::Lerp(4.75f, 3.167f, lerp);
					game_A->transform->SetLocalScale(scale_x, scale_y, 1);
				}
			}
		}
		
		break;
	}
	case States::POSTGAME:
	{
		player_dead->Revive(revive_percentatge);
		player_reviving->SetState(StateType::IDLE);
		Destroy(this->game_object);
		break;
	}
	}
}

void MiniGame_Revive::Minigame()
{
	float position_x = Maths::Lerp(-1.0f * sign, 1.0f * sign, (Time::GetGameTime() - time) / lerp_time);

	moving_part->transform->SetLocalPosition(float3(position_x, 0, 0));

	if (Input::GetKeyDown(SDL_SCANCODE_SPACE) || Input::GetControllerButtonDown(1, Input::CONTROLLER_BUTTON_A))
	{
		Effects();

		float points = 0.0f;
		points = (1 - Maths::Abs(position_x)) / input_times;
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
	// Bar Color
	color_time = Time::GetGameTime();
	moving_part->GetComponent<ComponentImage>()->SetBackgroundColor(1, 1, 0, 1);

	effects_change = true;
}

void MiniGame_Revive::StartMinigame(PlayerController* player_reviving)
{
	start_X->SetEnable(false);
	minigame->SetEnable(true);
	this->player_reviving = player_reviving;

	revive_state = States::MINIGAME;
}
