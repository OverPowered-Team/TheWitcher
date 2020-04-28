#include "UltiBar.h"
#include "../../Alien Engine/ComponentBar.h"

UltiBar::UltiBar() : Alien()
{
}

UltiBar::~UltiBar()
{
}

void UltiBar::Start()
{
	ultibar = game_object->GetChild("Ultibar");
	ultibar_charged = game_object->GetChild("Ultibar_Charged");
	controls = game_object->GetChild("Ulti_Controls");
	controls_lb = controls->GetChild("LB");
	controls_rb = controls->GetChild("RB");
	controls->SetEnable(false);
	ultibar->SetEnable(true);
	normal_ulti = ultibar->GetComponent<ComponentBar>();
	normal_ulti->SetBarValue(0);
	ultibar_charged->SetEnable(false);
}

void UltiBar::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_SPACE))
	{
		controls->SetEnable(true);
		actual_time = Time::GetGameTime();
		controls_lerping = true;
	}

	if (controls_lerping)
	{
		ControlsLerp();
	}
}

void UltiBar::UpdateBar(float actual_value)
{
	normal_ulti->SetBarValue(actual_value);
	if (actual_value == 0)
	{
		ultibar->SetEnable(true);
		ultibar_charged->SetEnable(false);
	}
}

void UltiBar::MaxBar()
{
	controls->SetEnable(true);
	normal_ulti->SetBarValue(1);
	ultibar->SetEnable(false);
	ultibar_charged->SetEnable(true);
	controls_lerping = true;
	actual_time = Time::GetGameTime();
}

void UltiBar::ControlsLerp()
{
	float t = (Time::GetGameTime() - actual_time) * (1 / time_to_lerp);
	float scale = 0.0f;
	float color = 0.f;

	if ((Time::GetGameTime() - actual_time) < (time_to_lerp * 0.5f))
	{
		scale = Maths::Lerp(3.f, 4.f, t);
		color = Maths::Lerp(0.275f, 0.85f, t);
	}
	else
	{
		scale = Maths::Lerp(4.f, 3.f, t);
		color = Maths::Lerp(0.85f, 0.275f, t);
	}

	controls_lb->transform->SetLocalScale(scale, scale, 1);
	controls_lb->GetComponent<ComponentImage>()->SetBackgroundColor(color, color, color, 1);
	controls_rb->transform->SetLocalScale(scale, scale, 1);
	controls_rb->GetComponent<ComponentImage>()->SetBackgroundColor(color, color, color, 1);

	if (t >= 1)
	{
		actual_time = Time::GetGameTime();
		++shining_count;

		if (shining_count >= times_shine)
		{
			shining_count = 0;
			controls_lerping = false;
		}
	}
}
