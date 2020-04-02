#include "Extra_Menus.h"
#include "Stat.h"
#include "PlayerController.h"

Extra_Menus::Extra_Menus() : Alien()
{
}

Extra_Menus::~Extra_Menus()
{
}

void Extra_Menus::Start()
{
	// Menus
	leaderboard = game_object->GetChild("Leaderboard");
	controls = game_object->GetChild("Controls");
	B = game_object->GetChild("B");
	settings = game_object->GetChild("Settings");
	leaderboard->SetEnable(false);
	controls->SetEnable(false);
	B->SetEnable(false);
	settings->SetEnable(false);


	Geralt_data = (PlayerController*)Geralt->GetComponentScript("PlayerController");
	damage1=(ComponentText*)game_object->GetChildRecursive("Damage_t1");
	kills1=(ComponentText*)game_object->GetChildRecursive("Kills_t1");
	score1=(ComponentText*)game_object->GetChildRecursive("Score_t1");
	relics1=(ComponentText*)game_object->GetChildRecursive("Relics1");

	Yennefer_data = (PlayerController*)Yennefer->GetComponentScript("PlayerController");
	damage2 = (ComponentText*)game_object->GetChildRecursive("Damage_t2");
	kills2 = (ComponentText*)game_object->GetChildRecursive("Kills_t2");
	score2 = (ComponentText*)game_object->GetChildRecursive("Score_t2");
	relics2 = (ComponentText*)game_object->GetChildRecursive("Relics2");
}

void Extra_Menus::PostUpdate()
{
	if ((Input::GetControllerButtonDown(1, Input::CONTROLLER_BUTTON_B) 
		|| Input::GetControllerButtonDown(2, Input::CONTROLLER_BUTTON_B)))
	{
		if (game_object->IsEnabled())
		{
			previous_menu->SetEnable(true);
			game_object->SetEnable(false);
		}
	}
}

void Extra_Menus::MenuSpawn(MENU menu_type)
{
	switch (menu_type) 
	{
	case MENU::CONTROLS:
		leaderboard->SetEnable(false);
		controls->SetEnable(true);
		B->SetEnable(true);
		settings->SetEnable(false);
		break;
	case MENU::SETTINGS:
		leaderboard->SetEnable(false);
		controls->SetEnable(false);
		B->SetEnable(false);
		settings->SetEnable(false);
		break;
	case MENU::LEADERBOARD:
		leaderboard->SetEnable(true);
		controls->SetEnable(false);
		B->SetEnable(true);
		settings->SetEnable(false);
		//score1->SetText(std::to_string((Geralt_data->player_data.total_kills * 150) + (Geralt_data->player_data.total_damage_dealt * 5) + (Geralt_data->relics.size() * 30)).data());
		/*damage1->SetText(std::to_string(Geralt_data->player_data.total_damage_dealt).data());
		kills1->SetText(std::to_string(Geralt_data->player_data.total_kills).data());
		relics1->SetText(std::to_string(Geralt_data->relics.size()).data());
		damage2->SetText(std::to_string(Yennefer_data->player_data.total_damage_dealt).data());
		kills2->SetText(std::to_string(Yennefer_data->player_data.total_kills).data());
		relics2->SetText(std::to_string(Yennefer_data->relics.size()).data());
		score2->SetText(std::to_string((Yennefer_data->player_data.total_kills * 150) + (Yennefer_data->player_data.total_damage_dealt * 5) + (Yennefer_data->relics.size() * 30)).data());*/
		break;
	}
}