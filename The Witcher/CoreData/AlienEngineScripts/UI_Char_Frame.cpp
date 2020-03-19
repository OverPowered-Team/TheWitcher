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

	lifebar_comp = (ComponentBar*)lifebar->GetComponent(ComponentType::UI_BAR);
	mana_bar_comp = (ComponentBar*)mana_bar->GetComponent(ComponentType::UI_BAR);
	xp_bar_comp = (ComponentBar*)xp_bar->GetComponent(ComponentType::UI_BAR);

}

void UI_Char_Frame::Update()
{
	if (Input::GetKeyRepeat(SDL_SCANCODE_9))
	{
		LifeChange(5);
	}

	if (Input::GetKeyRepeat(SDL_SCANCODE_8))
	{
		LifeChange(-5);
	}

	if (Input::GetKeyRepeat(SDL_SCANCODE_6))
	{
		ManaChange(5);
	}

	if (Input::GetKeyRepeat(SDL_SCANCODE_5))
	{
		ManaChange(-5);
	}

	if (Input::GetKeyRepeat(SDL_SCANCODE_3))
	{
		XpChange(5);
	}

	if (Input::GetKeyRepeat(SDL_SCANCODE_2))
	{
		XpChange(-5);
	}
}


void UI_Char_Frame::LifeChange(float life_change)
{
	float hp = 0.0f;
	hp = lifebar_comp->GetBarValue();

	// WHEN CHARACTER IS DONE
	// hp = character->life / character->max_life;
	life_change /= max_life;

	hp += life_change;
	lifebar_comp->SetBarValue(hp);
}

void UI_Char_Frame::ManaChange(float mana_change)
{
	float mana = 0.0f;
	mana = mana_bar_comp->GetBarValue();

	// WHEN CHARACTER IS DONE
	// hp = character->life / character->max_life;
	mana_change /= max_mana;

	mana += mana_change;

	mana_bar_comp->SetBarValue(mana);
}

void UI_Char_Frame::XpChange(float xp_change)
{
	float xp = 0.0f;
	xp = xp_bar_comp->GetBarValue();

	// WHEN CHARACTER IS DONE
	// hp = character->life / character->max_life;
	xp_change /= max_xp;

	xp += xp_change;

	xp_bar_comp->SetBarValue(xp);
}

