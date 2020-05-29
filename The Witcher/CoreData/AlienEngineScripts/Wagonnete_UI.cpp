#include "Wagonnete_UI.h"

Wagonnete_UI::Wagonnete_UI() : Alien()
{
}

Wagonnete_UI::~Wagonnete_UI()
{
}

void Wagonnete_UI::Start()
{
	lifebar = game_object->GetChild("Lifebar")->GetComponent<ComponentBar>();
	godmode = game_object->GetChild("Godmode");
	godmode->SetEnable(false);
}

void Wagonnete_UI::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_SPACE))
	{
		UpdateLifebar(50.f, 100.f);
	}

	if (changing_life)
	{
		float t = (Time::GetGameTime() - life_lerp_time) / lerping_time;
		float lerp = Maths::Lerp(actual_life, life_percentatge, t);

		lifebar->SetBarValue(lerp);

		if (has_been_hit)
		{
			float color_lerp = 0.0f;
			float red = 0.0f;

			if (t < 0.5f)
			{
				color_lerp = Maths::Lerp(1.0f, 0.f, t / lerping_time);
				red = Maths::Lerp(1.0f, 0.5f, t / lerping_time);
				lifebar->SetBarColor(red, color_lerp, color_lerp, 1);
			}
			else
			{
				color_lerp = Maths::Lerp(0.f, 1.f, t / lerping_time);
				red = Maths::Lerp(0.5f, 1.f, t / lerping_time);
				lifebar->SetBarColor(red, color_lerp, color_lerp, 1);
			}
		}

		if (t >= 1)
		{
			changing_life = false;
			lifebar->SetBarValue(life_percentatge);
			lifebar->SetBarColor(1, 1, 1, 1);
		}

	}
}

void Wagonnete_UI::UpdateLifebar(float new_life, float max_life)
{
	actual_life = lifebar->GetBarValue();
	life_percentatge = new_life / max_life;

	if (life_percentatge < actual_life)
	{
		has_been_hit = true;
	}
	else
	{
		has_been_hit = false;
	}

	changing_life = true;
	life_lerp_time = Time::GetGameTime();
}

