#include "UI_Char_Frame.h"

UI_Char_Frame::UI_Char_Frame() : Alien()
{
}

UI_Char_Frame::~UI_Char_Frame()
{
}

void UI_Char_Frame::Start()
{
	geralt_img = game_object->GetChild("Geralt_character_UI");
	yen_img = game_object->GetChild("Yennefer_character_UI");

	if (character == CHARACTER::YENNEFER)
	{
		portrait = yen_img->GetComponent<ComponentImage>();
		original_portrait_x = yen_img->transform->GetLocalPosition().x;
		original_portrait_y = yen_img->transform->GetLocalPosition().y;

		if (geralt_img->IsEnabled())
		{
			geralt_img->SetEnable(false);
		}
		if (!yen_img->IsEnabled())
		{
			yen_img->SetEnable(true);
		}
	}
	else
	{
		portrait = geralt_img->GetComponent<ComponentImage>();
		original_portrait_x = geralt_img->transform->GetLocalPosition().x;
		original_portrait_y = geralt_img->transform->GetLocalPosition().y;
		

		if (!geralt_img->IsEnabled())
		{
			geralt_img->SetEnable(true);

		}
		if (yen_img->IsEnabled())
		{
			yen_img->SetEnable(false);
		}
	}

	lifebar = game_object->GetChild("Lifebar")->GetComponent<ComponentBar>();
	mana_bar = game_object->GetChild("Mana")->GetComponent<ComponentBar>();
	kill_count = game_object->GetChild("Killcount");
	kill_count_number = kill_count->GetComponent<ComponentText>();
	kill_count_number_X = kill_count->GetChild("x")->GetComponent<ComponentText>();
	kill_count->SetEnable(false);
	fire = game_object->GetChild("Fire")->GetComponent<ComponentAnimatedImage>();
	fire->game_object_attached->SetEnable(false);
}

void UI_Char_Frame::Update()
{
	if (changing_life)
	{
		float t = (Time::GetGameTime() - time) * (1 / change_time);
		lifebar->SetBarValue(Maths::Lerp(now_life, life_change / max_life, t));

		if (player_hit)
		{
			shake_post_off_set += shake_off_set * Time::GetDT();

			portrait->game_object_attached->transform->SetLocalPosition(original_portrait_x + (Maths::PerlinNoise(0, shake_post_off_set, 0.8f, 0.8f) - 0.5f),
				original_portrait_y + (Maths::PerlinNoise(1, 0.8f, shake_post_off_set, 0.8f) - 0.5f),
				0.f);

			HitEffect(t);
		}

		if (t >= 1)
		{
			changing_life = false;
			player_hit = false;
			shake_post_off_set = shake_off_set;
			portrait->game_object_attached->transform->SetLocalPosition(original_portrait_x, original_portrait_y, 0.f);
		}
	}

	if (changing_chaos)
	{
		float t = (Time::GetGameTime() - chaos_time) * (1 / change_time);
		mana_bar->SetBarValue(Maths::Lerp(actual_chaos, chaos_change / max_chaos, t));

		if (t >= 1)
		{
			changing_chaos = false;
		}
	}

	if (low_life && (lifebar->GetBarValue() != 0))
	{
		LowLifeGlow();
	}

	if (kill_count->IsEnabled())
	{
		float t = (Time::GetGameTime() - killcount_lerp_time) / killcount_time_to_lerp;
		float lerp = 0.0f;

		switch (kc_state)
		{
		case KILL_COUNT_STATE::FADING_IN:
		{
			lerp = Maths::Lerp(0.f, 1.0f, t);
			break;
		}
		case KILL_COUNT_STATE::SHOWING:
		{
			lerp = 1;
			break;
		}
		case KILL_COUNT_STATE::FADING_OUT:
		{
			lerp = Maths::Lerp(1.f, 0.0f, t);
			break;
		}
		}

		// Set text alpha
		kill_count_number->SetAlpha(lerp);
		kill_count_number_X->SetAlpha(lerp);

		if (t >= 1)
		{
			switch (kc_state)
			{
			case KILL_COUNT_STATE::FADING_IN:
			{
				kc_state = KILL_COUNT_STATE::SHOWING;
				killcount_lerp_time = Time::GetGameTime() + 1.5f;
				break;
			}
			case KILL_COUNT_STATE::SHOWING:
			{
				kc_state = KILL_COUNT_STATE::FADING_OUT;
				killcount_lerp_time = Time::GetGameTime();
				break;
			}
			case KILL_COUNT_STATE::FADING_OUT:
			{
				kill_count->SetEnable(false);
				kc_state = KILL_COUNT_STATE::FADING_IN;
				break;
			}
			}
		}
	}

	if (is_fire_changing)
	{
		float t = (Time::GetGameTime() - fire_fade_time) / 0.25f;
		float lerp = Maths::Lerp(current_fire_alpha, goal_alpha_fire, t);

		fire->current_color.a = lerp;

		if (t >= 1)
		{
			fire->current_color.a = goal_alpha_fire;
			is_fire_changing = false;
			
			if (fire->current_color.a == 0)
			{
				fire->game_object_attached->SetEnable(false);
			}
		}
	}
}

// Bar Changes
void UI_Char_Frame::LifeChange(float actual_life, float max_life)
{
	float life_percentate = actual_life / max_life;
	if (life_percentate != lifebar->GetBarValue())
	{
		now_life = lifebar->GetBarValue();

		life_change = actual_life;
		this->max_life = max_life;

		if (life_percentate <= 0.35f)
		{
			low_life = true;
			low_life_glow_time = Time::GetGameTime();
		}
		else
		{
			low_life = false;
			lifebar->SetBarColor(1, 1, 1, 1);
		}

		if (now_life > life_percentate)
		{
			player_hit = true;
		}

		changing_life = true;
		time = Time::GetGameTime();
	}
}

void UI_Char_Frame::ManaChange(float mana_change, float max_mana)
{
	if ((mana_change / max_mana) != mana_bar->GetBarValue())
	{
		actual_chaos = mana_bar->GetBarValue();

		chaos_change = mana_change;
		max_chaos = max_mana;

		changing_chaos = true;
		chaos_time = Time::GetGameTime();
	}
}

void UI_Char_Frame::StartFadeKillCount(int new_kill_count)
{
	if (new_kill_count >= 10)
	{
		kill_count_number->SetText(std::to_string(new_kill_count).c_str());
	}
	else
	{
		std::string kills = "0" + std::to_string(new_kill_count);
		kill_count_number->SetText(kills.c_str());
	}

	kill_count->SetEnable(true);

	switch (kc_state)
	{
	case KILL_COUNT_STATE::FADING_IN:
	{
		if (((Time::GetGameTime() - killcount_lerp_time) / killcount_time_to_lerp) >= 1.f)
		{
			kill_count_number->SetAlpha(0);
			kill_count_number_X->SetAlpha(0);
			killcount_lerp_time = Time::GetGameTime();
		}
		break;
	}
	case KILL_COUNT_STATE::SHOWING:
	{
		killcount_lerp_time = Time::GetGameTime() + 1.5f;
		break;
	}
	case KILL_COUNT_STATE::FADING_OUT:
	{
		kc_state = KILL_COUNT_STATE::FADING_IN;
		killcount_lerp_time = Time::GetGameTime() - kill_count_number->current_color.a * killcount_time_to_lerp;
		break;
	}
	}
}

void UI_Char_Frame::PlayerOnFire(bool is_on_fire)
{
	if (!fire->game_object_attached->IsEnabled())
	{
		fire->game_object_attached->SetEnable(true);
	}

	if (is_on_fire != fire->current_color.a)
	{
		goal_alpha_fire = is_on_fire;
		current_fire_alpha = fire->current_color.a;

		is_fire_changing = true;
		fire_fade_time = Time::GetGameTime();
	}
}

// Effects
void UI_Char_Frame::HitEffect(float lerp_time)
{
	if (lerp_time >= 1.f)
	{
		lifebar->SetBarColor(1.0f, 1.0f, 1.0f, 1.f);
		portrait->SetBackgroundColor(1.0f, 1.0f, 1.0f, 1.f);
		return;
	}

	float lerp_life = 0.0f;
	float blood_color = 0.0f;

	if (lerp_time <= 0.5f)
	{
		lerp_life = Maths::Lerp(1.0f, 0.f, lerp_time * 2);
		blood_color = Maths::Lerp(1.0f, 0.5f, lerp_time * 2);
	}
	else
	{
		lerp_life = Maths::Lerp(0.f, 1.0f, (lerp_time - 0.5f) * 2);
		blood_color = Maths::Lerp(0.5f, 1.0f, (lerp_time - 0.5f) * 2);
	}

	lifebar->SetBarColor(blood_color, lerp_life, lerp_life, 1.f);
	portrait->SetBackgroundColor(blood_color, lerp_life, lerp_life, 1.f);
	
}

void UI_Char_Frame::LowLifeGlow()
{
	float color = 0.0f;
	float blood_color = 0.0f;
	float t = (Time::GetGameTime() - low_life_glow_time);

	if (low_life_sign < 0)
	{
		color = Maths::Lerp(1.0f, 0.f, t);
		blood_color = Maths::Lerp(1.0f, 0.5f, t);
	}
	else
	{
		color = Maths::Lerp(0.f, 1.f, t);
		blood_color = Maths::Lerp(0.5f, 1.f, t);
	}

	lifebar->SetBarColor(blood_color, color, color, 1);

	if (t >= 1)
	{
		low_life_sign = -low_life_sign;
		low_life_glow_time = Time::GetGameTime();
	}
}
