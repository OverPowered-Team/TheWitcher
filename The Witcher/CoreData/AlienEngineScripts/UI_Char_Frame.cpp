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
	//mana_bar = (ComponentBar*)game_object->GetChild("")->GetComponent(ComponentType::UI_BAR);
	//xp_bar = (ComponentBar*)game_object->GetChild("Lifebar")->GetComponent(ComponentType::UI_BAR);

}

void UI_Char_Frame::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_A))
	{
		lifebar->SetBarValue(lifebar->GetBarValue() - 0.05f);
	}

	if (Input::GetKeyDown(SDL_SCANCODE_D))
	{
		lifebar->SetBarValue(lifebar->GetBarValue() + 0.05f);
	}
}