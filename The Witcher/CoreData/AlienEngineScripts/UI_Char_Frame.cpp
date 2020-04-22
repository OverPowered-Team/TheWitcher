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
		if (!geralt_img->IsEnabled())
		{
			geralt_img->SetEnable(true);
		}
		if (yen_img->IsEnabled())
		{
			yen_img->SetEnable(false);
		}
	}

	lifebar = game_object->GetChild("Lifebar");
	mana_bar = game_object->GetChild("Mana");
	xp_bar = game_object->GetChild("Xp");

	lifebar_comp = lifebar->GetComponent<ComponentBar>();
	mana_bar_comp = mana_bar->GetComponent<ComponentBar>();
	xp_bar_comp = xp_bar->GetComponent<ComponentBar>();

}


void UI_Char_Frame::LifeChange(float actual_life, float max_life)
{
	lifebar_comp->SetBarValue(actual_life / max_life);
}

void UI_Char_Frame::ManaChange(float mana_change, float max_mana)
{
	mana_bar_comp->SetBarValue(mana_change / max_mana);
}

void UI_Char_Frame::XpChange(float xp_change, float max_xp)
{
	xp_bar_comp->SetBarValue(xp_change/max_xp);
}

