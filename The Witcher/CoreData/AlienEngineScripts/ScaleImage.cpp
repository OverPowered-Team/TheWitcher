#include "ScaleImage.h"

ScaleImage::ScaleImage() : Alien()
{
}

ScaleImage::~ScaleImage()
{
}

void ScaleImage::Start()
{
	//start_time = Time::GetTimeSinceStart();
	start_scale = transform->GetLocalScale();
}

void ScaleImage::Update()
{
	if (first_start)
	{
		start_time = Time::GetGameTime();
		first_start = false;
	}

	float delta_time = Time::GetGameTime() - start_time;
	float porcetage = delta_time / duration;
	
	if (porcetage <= 1)
	{
		
		transform->SetLocalScale(Lerp(start_scale, scale, porcetage));
	}
	
}