#include "UI_Char_Frame.h"
#include "../../Alien Engine/ComponentBar.h"

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
}

void UI_Char_Frame::Update()
{
	if (changing_life)
	{
		float t = (Time::GetGameTime() - time) * (1 / change_time);
		lifebar->SetBarValue(Maths::Lerp(now_life, life_change / max_life, t));

		if (player_hit)
		{
			HitEffect(t);
		}

		if (t >= 1)
		{
			changing_life = false;
			player_hit = false;
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

		if (life_percentate <= 0.15f)
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

// Effects
void UI_Char_Frame::HitEffect(float lerp_time)
{
	float lerp_portrait = 0.0f;
	float lerp_life = 0.0f;

	if (lerp_time <= 0.5f)
	{
		lerp_portrait = Maths::Lerp(1.0f, 0.f, lerp_time);
		lerp_life = Maths::Lerp(1.0f, 0.150f, lerp_time);
	}
	else
	{
		lerp_portrait = Maths::Lerp(0.f, 1.0f, lerp_time);
		lerp_life = Maths::Lerp(0.150f, 1.f, lerp_time);
	}

	portrait->SetBackgroundColor(1.f, lerp_portrait, lerp_portrait, 1.f);
	lifebar->SetBarColor(1.f, lerp_life, lerp_life, 1.f);
}

void UI_Char_Frame::LowLifeGlow()
{
	float color = 0.0f;
	float t = (Time::GetGameTime() - low_life_glow_time);

	if (low_life_sign < 0)
	{
		color = Maths::Lerp(1.0f, 0.f, t);
	}
	else
	{
		color = Maths::Lerp(0.f, 1.f, t);
	}

	lifebar->SetBarColor(1, color, color, 1);

	if (t >= 1)
	{
		low_life_sign = -low_life_sign;
		low_life_glow_time = Time::GetGameTime();
	}
}
