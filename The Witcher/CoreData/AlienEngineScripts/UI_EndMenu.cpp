#include "UI_EndMenu.h"

UI_EndMenu::UI_EndMenu() : Alien()
{
}

UI_EndMenu::~UI_EndMenu()
{
}

void UI_EndMenu::Start()
{
	damage_info = GameObject::FindWithName("DamageInfo");
	kills_info = GameObject::FindWithName("KillsInfo");
	relics_info = GameObject::FindWithName("RelicsInfo");
	gold_info = GameObject::FindWithName("GoldInfo");
	xp_info = GameObject::FindWithName("XPInfo");
}

void UI_EndMenu::EndMenu(ENDMENU menu)
{
	if (first_time)
	{
		switch (menu)
		{
		case UI_EndMenu::ENDMENU::DAMAGE:
			damage_info->SetEnable(true);
			kills_info->SetEnable(false);
			relics_info->SetEnable(false);
			gold_info->SetEnable(false);
			xp_info->SetEnable(false);
			break;
		case UI_EndMenu::ENDMENU::KILLS:
			damage_info->SetEnable(false);
			kills_info->SetEnable(true);
			relics_info->SetEnable(false);
			gold_info->SetEnable(false);
			xp_info->SetEnable(false);
			break;
		case UI_EndMenu::ENDMENU::RELICS:
			damage_info->SetEnable(false);
			kills_info->SetEnable(false);
			relics_info->SetEnable(true);
			gold_info->SetEnable(false);
			xp_info->SetEnable(false);
			break;
		case UI_EndMenu::ENDMENU::GOLD:
			damage_info->SetEnable(false);
			kills_info->SetEnable(false);
			relics_info->SetEnable(false);
			gold_info->SetEnable(true);
			xp_info->SetEnable(false);
			break;
		case UI_EndMenu::ENDMENU::XP:
			damage_info->SetEnable(false);
			kills_info->SetEnable(false);
			relics_info->SetEnable(false);
			gold_info->SetEnable(false);
			xp_info->SetEnable(true);
			break;
		}
		first_time = false;
	}
}

void UI_EndMenu::ExitButton()
{
	first_time = true;
}


