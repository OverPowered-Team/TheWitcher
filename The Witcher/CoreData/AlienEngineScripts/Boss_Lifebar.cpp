#include "Boss_Lifebar.h"

Boss_Lifebar::Boss_Lifebar() : Alien()
{
}

Boss_Lifebar::~Boss_Lifebar()
{
}

void Boss_Lifebar::Start()
{
	lifebar = game_object->GetChild("Boss_Life")->GetComponent<ComponentBar>();

	boss_name = lifebar->game_object_attached->GetChild("Text")->GetComponent<ComponentText>();
	boss_name->SetText(game_object->parent->GetName());

	lifebar->SetBackgroundColor(1, 1, 1, 0.0f);
	lifebar->SetBarColor(1, 1, 1, 0.0f);
	boss_name->SetAlpha(0.0f);
}

void Boss_Lifebar::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_SPACE))
	{
		ShowLifeBar(true);
	}

	if (Input::GetKeyDown(SDL_SCANCODE_LSHIFT))
	{
		ShowLifeBar(false);
	}

	if (lifebar_changing)
	{
		float t = (Time::GetTimeSinceStart() - start_lerp_time) / time_to_lerp;
		float lerp = Maths::Lerp(start_life, goal_life, t);

		lifebar->SetBarValue(lerp);

		if (t >= 1)
		{
			lifebar->SetBarValue(goal_life);
			lifebar_changing = false;
		}
	}

	if (changing_alpha)
	{
		float t = (Time::GetTimeSinceStart() - start_time_show) / time_to_show;
		float lerp = Maths::Lerp(starting_point, ending_point, t);

		lifebar->SetBackgroundColor(1, 1, 1, lerp);
		lifebar->SetBarColor(1, 1, 1, lerp);
		boss_name->SetAlpha(lerp);

		if (t >= 1)
		{
			if (lerp >= 1)
			{
				lifebar->SetBackgroundColor(1, 1, 1, 1.f);
				boss_name->SetAlpha(1.0f);
			}
			else
			{
				lifebar->SetBackgroundColor(1, 1, 1, 0.f);
				boss_name->SetAlpha(0.0f);
			}

			changing_alpha = false;
		}
	}
}

void Boss_Lifebar::UpdateLifebar(float life, float max_life)
{
	goal_life = life / max_life;
	start_life = lifebar->GetBarValue();
	lifebar_changing = true;
	
	start_lerp_time = Time::GetTimeSinceStart();
}

void Boss_Lifebar::ShowLifeBar(bool want_to_show)
{
	if (want_to_show)
	{
		starting_point = 0.0f;
		ending_point = 1.0f;
	}
	else
	{
		starting_point = 1.0f;
		ending_point = 0.0f;
	}

	changing_alpha = true;
	start_time_show = Time::GetTimeSinceStart();
}

