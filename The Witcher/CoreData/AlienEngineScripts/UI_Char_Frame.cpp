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
	xp_bar = game_object->GetChild("Xp")->GetComponent<ComponentBar>();
}

void UI_Char_Frame::Update()
{
	if (changing_life)
	{
		float t = (Time::GetGameTime() - time) * (1 / change_time);
		lifebar->SetBarValue(Maths::Lerp(now_life, life_change / max_life, t));

		if (player_hit)
		{
			float lerp = 0.0f;

			if (t <= 0.5f)
			{
				lerp = Maths::Lerp(1.0f, 0.f, t);
			}
			else
			{
				lerp = Maths::Lerp(0.f, 1.0f, t);
			}

			portrait->SetBackgroundColor(1.f, lerp, lerp, 1.f);
		}

		if (t >= 1)
		{
			changing_life = false;
			player_hit = false;
		}
	}
}


void UI_Char_Frame::LifeChange(float actual_life, float max_life)
{
	now_life = lifebar->GetBarValue();

	life_change = actual_life;
	this->max_life = max_life;

	if (now_life > (actual_life / max_life))
	{
		player_hit = true;
	}

	changing_life = true;
	time = Time::GetGameTime();
}

void UI_Char_Frame::ManaChange(float mana_change, float max_mana)
{
	//mana_bar_comp->SetBarValue(mana_change / max_mana);
}
