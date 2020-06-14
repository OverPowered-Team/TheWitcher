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

	duck = game_object->GetChild("Duck")->GetComponent<ComponentImage>();
	dodge = game_object->GetChild("Dodge")->GetComponent<ComponentImage>();
	duck->current_color.a = 0;
	dodge->current_color.a = 0;
}

void Wagonnete_UI::Update()
{
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
				red = Maths::Lerp(1.0f, 0.25f, t / lerping_time);
				lifebar->SetBarColor(red, color_lerp, color_lerp, 1);
			}
			else
			{
				color_lerp = Maths::Lerp(0.f, 1.f, t / lerping_time);
				red = Maths::Lerp(0.25f, 1.f, t / lerping_time);
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

	if (duck_changing)
	{
		HandleImage(duck);
	}

	if (dodge_changing)
	{
		HandleImage(dodge);
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

void Wagonnete_UI::ActivateTrigger(Wagonnette_Triggers::TRIGGER_TYPE type)
{
	switch (type)
	{
	case Wagonnette_Triggers::TRIGGER_TYPE::DUCKING:
	{
		duck_changing = true;
		duck_state = TRANSITION_STATE::FADE_IN;
		duck_time = Time::GetGameTime();
		break;
	}
	case Wagonnette_Triggers::TRIGGER_TYPE::DODGING:
	{
		dodge_changing = true;
		dodge_state = TRANSITION_STATE::FADE_IN;
		dodge_time = Time::GetGameTime();
		break;
	}
	}
}

void Wagonnete_UI::HandleImage(ComponentImage* image)
{
	TRANSITION_STATE* state = nullptr;
	float* time = nullptr;
	bool* showing = nullptr;

	if (image == duck)
	{
		state = &duck_state;
		time = &duck_time;
		showing = &duck_changing;
	}
	else
	{
		state = &dodge_state;
		time = &dodge_time;
		showing = &dodge_changing;
	}

	switch ((*state))
	{
	case TRANSITION_STATE::FADE_IN:
	{
		float t = (Time::GetGameTime() - (*time)) / 0.5f;
		float lerp = Maths::Lerp(0.f, 1.f, t);

		image->current_color.a = lerp;

		if (t >= 1)
		{
			image->current_color.a = 1;
			(*state) = TRANSITION_STATE::SHOWING;
			(*time) = Time::GetGameTime();
		}

		break;
	}
	case TRANSITION_STATE::SHOWING:
	{
		if ((*time) + 2 < Time::GetGameTime())
		{
			(*state) = TRANSITION_STATE::FADE_OUT;
			(*time) = Time::GetGameTime();
		}
		break;
	}
	case TRANSITION_STATE::FADE_OUT:
	{
		float t = (Time::GetGameTime() - (*time)) / 0.5f;
		float lerp = Maths::Lerp(1.0f, 0.f, t);

		image->current_color.a = lerp;

		if (t >= 1)
		{
			image->current_color.a = 0;
			(*showing) = false;
		}
		break;
	}
	}
}

