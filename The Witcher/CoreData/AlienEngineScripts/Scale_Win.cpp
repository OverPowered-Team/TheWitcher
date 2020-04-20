#include "Scale_Win.h"

Scale_Win::Scale_Win() : Alien()
{
}

Scale_Win::~Scale_Win()
{
}

void Scale_Win::Start()
{
	left_scale = game_object->GetChild("Left_Scale");
	right_scale = game_object->GetChild("Right_Scale");
}

void Scale_Win::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_P))
	{
		CalculateInclination();
	}

	// Waiting on new release
	/*if (!in_place)
	{
		left_scale->transform->SetLocalPosition(7.5f, math::Lerp(original_position1,desired_position1,(Time::GetGameTime()-time)/time_to_scale), 0);
		right_scale->transform->SetLocalPosition(-7.5f, math::Lerp(original_position2, desired_position2, (Time::GetGameTime() - time) / time_to_scale), 0);

		if (Time::GetGameTime() > time + time_to_scale)
		{ 
			in_place = true;
		}
	}*/
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

	// TO DELETE
	left_scale->transform->SetLocalPosition(7.5f, desired_position1, 0);
	right_scale->transform->SetLocalPosition(-7.5f, desired_position2, 0);
	// ----------------------------------------

	in_place = false;
	time = Time::GetGameTime();
}
