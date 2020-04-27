#include "MiniGame_Revive.h"

MiniGame_Revive::MiniGame_Revive() : Alien()
{
}

MiniGame_Revive::~MiniGame_Revive()
{
}

void MiniGame_Revive::Start()
{
	moving_part = game_object->GetChild("Moving");
	text = GameObject::FindWithName("Text");
	text->SetEnable(false);
}

void MiniGame_Revive::Update()
{
	float position_x = Maths::Lerp(-1.0f * sign, 1.0f * sign, Time::GetGameTime() - time);

	moving_part->transform->SetLocalPosition(float3(position_x, 0, 0));

	if (Input::GetKeyDown(SDL_SCANCODE_SPACE))
	{
		float points = 0.0f;
		points = (1 - Maths::Abs(position_x)) / input_times;
		++actual_inputs;
		revive_percentatge += points;

		LOG("Points: %f", points);
		LOG("Inputs: %f", actual_inputs);
		LOG("Total Revive: %f", revive_percentatge);

		if (actual_inputs == input_times)
		{
			game_object->SetEnable(false);
			text->SetEnable(true);
			text->GetComponent<ComponentText>()->SetText(std::to_string(revive_percentatge).c_str());
		}
	}

	if (lerp_time + time < Time::GetGameTime())
	{
		sign = -sign;
		time = Time::GetGameTime();
	}
}