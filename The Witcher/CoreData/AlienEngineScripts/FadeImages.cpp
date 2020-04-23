#include "FadeImages.h"

FadeImages::FadeImages() : Alien()
{
}

FadeImages::~FadeImages()
{
}

void FadeImages::Start()
{
	image = GetComponent<ComponentImage>();
	
	start_time_scene = Time::GetGameTime();
	if (image == nullptr)
	{
		LOG("Component Image not found");
		return;
	}
	current_time = Time::GetGameTime() - start_time_scene;
}

void FadeImages::Update()
{
	if (image == nullptr)
	{
		LOG("Component Image not found");
		return;
	}
	if (activated_event== false && start_next_image < current_time)
	{
		//Tween::TweenUIAlpha(game_object, 0, transition_duration, Tween::linear);
		Tween::TweenUIAlpha(second_image, 1, transition_duration, Tween::linear);
		activated_event = true;
	}
	current_time = Time::GetGameTime() - start_time_scene;
}