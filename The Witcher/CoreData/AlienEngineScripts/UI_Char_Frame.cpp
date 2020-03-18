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
	geralt_img = game_object->GetChild("Geralt");
	yen_img = game_object->GetChild("Yennefer");

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

	lifebar = (ComponentBar*)game_object->GetChild("Lifebar")->GetComponent(ComponentType::UI_BAR);
	mana_bar = (ComponentBar*)game_object->GetChild("Mana")->GetComponent(ComponentType::UI_BAR);
	xp_bar = (ComponentBar*)game_object->GetChild("Xp")->GetComponent(ComponentType::UI_BAR);

}

void UI_Char_Frame::Update()
{
	if (Input::GetKeyRepeat(SDL_SCANCODE_D))
	{
		LifeChange(5);
	}

	if (Input::GetKeyRepeat(SDL_SCANCODE_A))
	{
		LifeChange(-5);
	}

	if (Input::GetKeyRepeat(SDL_SCANCODE_W))
	{
		ManaChange(5);
	}

	if (Input::GetKeyRepeat(SDL_SCANCODE_Q))
	{
		ManaChange(-5);
	}

	if (Input::GetKeyRepeat(SDL_SCANCODE_X))
	{
		XpChange(5);
	}

	if (Input::GetKeyRepeat(SDL_SCANCODE_Z))
	{
		XpChange(-5);
	}
}


void UI_Char_Frame::LifeChange(float life_change)
{
	float hp = 0.0f;
	hp = lifebar->GetBarValue();

	// WHEN CHARACTER IS DONE
	// hp = character->life / character->max_life;
	life_change /= max_life;

	hp += life_change;
	lifebar->SetBarValue(hp);
}

void UI_Char_Frame::ManaChange(float mana_change)
{
	float mana = 0.0f;
	mana = mana_bar->GetBarValue();

	// WHEN CHARACTER IS DONE
	// hp = character->life / character->max_life;
	mana_change /= max_mana;

	mana += mana_change;

	mana_bar->SetBarValue(mana);
}

void UI_Char_Frame::XpChange(float xp_change)
{
	float xp = 0.0f;
	xp = xp_bar->GetBarValue();

	// WHEN CHARACTER IS DONE
	// hp = character->life / character->max_life;
	xp_change /= max_xp;

	xp += xp_change;

	xp_bar->SetBarValue(xp);
}

