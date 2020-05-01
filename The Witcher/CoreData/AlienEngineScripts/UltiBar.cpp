#include "UltiBar.h"

UltiBar::UltiBar() : Alien()
{
}

UltiBar::~UltiBar()
{
}

void UltiBar::Start()
{
	// GameObjects
	ultibar = game_object->GetChild("Ultibar");
	ultibar_charged = game_object->GetChild("Ultibar_Charged");
	normal_ulti = ultibar->GetComponent<ComponentBar>();
	ultimate_bar = ultibar_charged->GetComponent<ComponentBar>();

	// Controls
	controls = game_object->GetChild("Ulti_Controls");
	controls_lb = controls->GetChild("LeftControl");
	controls_rb = controls->GetChild("RightControl");
	lb_image = controls_lb->GetComponent<ComponentImage>();
	rb_image = controls_rb->GetComponent<ComponentImage>();
	pre_scale = controls_lb->transform->GetLocalScale().x;

	// Enables Setting
	controls->SetEnable(false);
	ultibar_charged->SetEnable(false);
	ultibar->SetEnable(true);
	normal_ulti->SetBarValue(0);
}

void UltiBar::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_SPACE))
	{
		MaxBar();
	}
	if (Input::GetKeyDown(SDL_SCANCODE_LSHIFT))
	{
		UpdateBar(0);
	}

	// Bar Charges With Lerp
	if (bar_charging)
	{
		float t = (Time::GetGameTime() - bar_charging_time) * (1 / ulti_increase_lerp_time);
		float lerp = Maths::Lerp(previous_bar_value, new_bar_value, t);

		normal_ulti->SetBarValue(lerp);

		if (t >= 1)
		{
			bar_charging = false;

			if (is_max)
			{
				// Data Setting
				is_max = false;
				controls_lerping = true;

				// Enables Setting
				ultibar->SetEnable(false);
				controls->SetEnable(true);
				ultibar_charged->SetEnable(true);

				// Lerping Time Settings
				glow_time = Time::GetGameTime();
				actual_time = Time::GetGameTime();
			}
		}
	}

	if (controls_lerping)
	{
		ControlsLerp();
	}

	// Charged Bar Glowing
	if (ultibar_charged->IsEnabled())
	{
		float t = (Time::GetGameTime() - glow_time);
		float color = 0.0f;

		if (glowing)
		{
			color = Maths::Lerp(1.f, 0.6f, t);
		}
		else
		{
			color = Maths::Lerp(0.6f, 1.0f, t);
		}

		ultimate_bar->SetBarColor(color, color, color, 1);

		if (t >= 1)
		{
			glowing = !glowing;
			glow_time = Time::GetGameTime();
		}
	}
}

void UltiBar::UpdateBar(float actual_value)
{
	if (actual_value == 0)
	{
		if (controls->IsEnabled())
		{
			controls->SetEnable(false);
		}

		ultibar->SetEnable(true);
		ultibar_charged->SetEnable(false);
	}

	previous_bar_value = normal_ulti->GetBarValue();
	new_bar_value = actual_value;
	bar_charging = true;
	bar_charging_time = Time::GetGameTime();
}

void UltiBar::MaxBar()
{
	// Values Setting
	is_max = true;
	UpdateBar(1);
}

void UltiBar::ControlsLerp()
{
	float t = (Time::GetGameTime() - actual_time) * (1 / time_to_lerp_controls);
	float scale = 0.0f;
	float color = 0.f;

	if ((Time::GetGameTime() - actual_time) < (time_to_lerp_controls * 0.5f))
	{
		scale = Maths::Lerp(pre_scale, (pre_scale * (1.f + 1.0f / 3.0f)), t);
		color = Maths::Lerp(0.275f, 0.85f, t);
	}
	else
	{
		scale = Maths::Lerp(pre_scale * (1.f + 1.0f / 3.0f), pre_scale, t);
		color = Maths::Lerp(0.85f, 0.275f, t);
	}

	controls_lb->transform->SetLocalScale(scale, scale, 1);
	lb_image->SetBackgroundColor(color, color, color, 1);
	controls_rb->transform->SetLocalScale(scale, scale, 1);
	rb_image->SetBackgroundColor(color, color, color, 1);

	if (t >= 1)
	{
		++shining_count;
		actual_time = Time::GetGameTime();

		if (shining_count >= times_shine)
		{
			shining_count = 0;
			controls_lerping = false;
			controls->SetEnable(false);
		}
	}
}
