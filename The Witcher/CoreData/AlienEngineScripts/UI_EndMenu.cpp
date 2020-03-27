#include "UI_EndMenu.h"

UI_EndMenu::UI_EndMenu() : Alien()
{
}

UI_EndMenu::~UI_EndMenu()
{
}

void UI_EndMenu::EndMenu(ENDMENU menu)
{
	switch (menu)
	{
	case UI_EndMenu::ENDMENU::DAMAGE:
		GameObject::FindWithName("DamageInfo")->SetEnable(true);
		GameObject::FindWithName("KillsInfo")->SetEnable(false);
		GameObject::FindWithName("RelicsInfo")->SetEnable(false);
		GameObject::FindWithName("GoldInfo")->SetEnable(false);
		GameObject::FindWithName("XPInfo")->SetEnable(false);
		break;
	case UI_EndMenu::ENDMENU::KILLS:
		GameObject::FindWithName("DamageInfo")->SetEnable(false);
		GameObject::FindWithName("KillsInfo")->SetEnable(true);
		GameObject::FindWithName("RelicsInfo")->SetEnable(false);
		GameObject::FindWithName("GoldInfo")->SetEnable(false);
		GameObject::FindWithName("XPInfo")->SetEnable(false);
		break;
	case UI_EndMenu::ENDMENU::RELICS:
		GameObject::FindWithName("DamageInfo")->SetEnable(false);
		GameObject::FindWithName("KillsInfo")->SetEnable(false);
		GameObject::FindWithName("RelicsInfo")->SetEnable(true);
		GameObject::FindWithName("GoldInfo")->SetEnable(false);
		GameObject::FindWithName("XPInfo")->SetEnable(false);
		break;
	case UI_EndMenu::ENDMENU::GOLD:
		GameObject::FindWithName("DamageInfo")->SetEnable(false);
		GameObject::FindWithName("KillsInfo")->SetEnable(false);
		GameObject::FindWithName("RelicsInfo")->SetEnable(false);
		GameObject::FindWithName("GoldInfo")->SetEnable(true);
		GameObject::FindWithName("XPInfo")->SetEnable(false);
		break;
	case UI_EndMenu::ENDMENU::XP:
		GameObject::FindWithName("DamageInfo")->SetEnable(false);
		GameObject::FindWithName("KillsInfo")->SetEnable(false);
		GameObject::FindWithName("RelicsInfo")->SetEnable(false);
		GameObject::FindWithName("GoldInfo")->SetEnable(false);
		GameObject::FindWithName("XPInfo")->SetEnable(true);
		break;
	}
}


