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
	normal_ulti = ultibar->GetComponent<ComponentBar>();

	// Controls
	controls = game_object->GetChild("Ulti_Controls");
	controls_lb = controls->GetChild("LeftControl");
	controls_rb = controls->GetChild("RightControl");
	lb_image = controls_lb->GetComponent<ComponentImage>();
	rb_image = controls_rb->GetComponent<ComponentImage>();
	pre_scale = controls_lb->transform->GetLocalScale().x;

	// Enables Setting
	controls->SetEnable(false);
	ultibar->SetEnable(true);
	normal_ulti->SetBarValue(0);
}

void UltiBar::Update()
{
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
				is_bar_charged = true;

				// Enables Setting
				controls->SetEnable(true);

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
	if (is_bar_charged)
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

		normal_ulti->SetBarColor(color, color, color, 1);

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
		is_bar_charged = false;
	}
	else if (actual_value == 1)
	{
		is_max = true;
	}

	previous_bar_value = normal_ulti->GetBarValue();
	new_bar_value = actual_value;
	bar_charging = true;
	bar_charging_time = Time::GetGameTime();
}

void UltiBar::ControlsLerp()
{
	float t = (Time::GetGameTime() - actual_time) * (1 / time_to_lerp_controls);
	float scale = 0.0f;
	float color = 0.f;

	if ((Time::GetGameTime() - actual_time) < (time_to_lerp_controls * 0.5f))
	{
		scale = Maths::Lerp(pre_scale, (pre_scale * (1.f + 1.0f / 6.0f)), t * 2);
		color = Maths::Lerp(0.275f, 0.425f, t * 2);
	}
	else
	{
		scale = Maths::Lerp(pre_scale * (1.f + 1.0f / 6.0f), pre_scale, (t - 0.5f) * 2);
		color = Maths::Lerp(0.425f, 0.275f, (t - 0.5f) * 2);
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
