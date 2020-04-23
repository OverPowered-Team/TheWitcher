#include "MoveImages.h"

MoveImages::MoveImages() : Alien()
{
}

MoveImages::~MoveImages()
{
}

void MoveImages::Start()
{
	start_time = Time::GetGameTime();
}

void MoveImages::Update()
{
	current_time = Time::GetGameTime() - start_time;
	if (!started_event && current_time > start_movement)
	{
		Tween::TweenMoveTo(game_object, nextPos->transform->GetGlobalPosition(), transition_duration, Tween::linear);
		started_event = true;
	}
}