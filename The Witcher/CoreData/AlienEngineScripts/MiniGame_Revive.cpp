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
	text = GameObject::FindWithName("Text");
	text->SetEnable(false);
}

void MiniGame_Revive::Update()
{
	switch (revive_state)
	{
	case States::PREGAME:
	{
		if (Input::GetControllerButtonDown(1, Input::CONTROLLER_BUTTON_X))
		{
			start_X->SetEnable(false);
			minigame->SetEnable(true);

			revive_state = States::MINIGAME;
		}
		break;
	}
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
		}
		
		break;
	}
	case States::POSTGAME:
	{
		// Revive player
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

		LOG("Points: %f", points);
		LOG("Inputs: %i", actual_inputs);
		LOG("Total Revive: %f", revive_percentatge);
	}

	if (color_changed && (color_time + 0.1f < Time::GetGameTime()))
	{
		moving_part->GetComponent<ComponentImage>()->SetBackgroundColor(1, 1, 1, 1);
		color_changed = false;
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
	color_changed = true;

	// A Button Lerp

}
