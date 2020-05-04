#include "PauseMenu_Buttons.h"
#include "InGame_UI.h"
#include "Extra_Menus.h"
#include "..\..\ComponentText.h"
#include "Scores_Data.h"

PauseMenu_Buttons::PauseMenu_Buttons() : Alien()
{
}

PauseMenu_Buttons::~PauseMenu_Buttons()
{
}

void PauseMenu_Buttons::Update()
{
	if ((Input::GetControllerButtonDown(1, Input::CONTROLLER_BUTTON_B) 
		|| Input::GetControllerButtonDown(2, Input::CONTROLLER_BUTTON_B)))
	{
		if (GameObject::FindWithName("Menu")->IsEnabled())
		{
			GameObject::FindWithName("UI_InGame")->GetComponent<InGame_UI>()->PauseMenu(false);
		}
	}
}

void PauseMenu_Buttons::Resume()
{
	GameObject::FindWithName("UI_InGame")->GetComponent<InGame_UI>()->PauseMenu(false);
}

void PauseMenu_Buttons::Leaderboard()
{
	GameObject::FindWithName("Extra_Menus")->SetEnable(true);
	GameObject::FindWithName("Extra_Menus")->GetComponent<Extra_Menus>()->MenuSpawn(Extra_Menus::MENU::LEADERBOARD);
	GameObject::FindWithName("Menu")->SetEnable(false);
}

void PauseMenu_Buttons::Controls()
{
	GameObject::FindWithName("Extra_Menus")->SetEnable(true);
	GameObject::FindWithName("Extra_Menus")->GetComponent<Extra_Menus>()->MenuSpawn(Extra_Menus::MENU::CONTROLS);
	GameObject::FindWithName("Menu")->SetEnable(false);
}

void PauseMenu_Buttons::Settings()
{
}

void PauseMenu_Buttons::RetryLevel()
{
	Scores_Data::last_checkpoint_position = float3::inf();
	GameObject::FindWithName("UI_InGame")->GetComponent<InGame_UI>()->PauseMenu(false);
	SceneManager::LoadScene("Lvl_1_Art_Colliders");
}

void PauseMenu_Buttons::Exit_to_Menu()
{
	GameObject::FindWithName("UI_InGame")->GetComponent<InGame_UI>()->PauseMenu(false);
	SceneManager::LoadScene("Lvl_1_Art_Colliders");
}

void PauseMenu_Buttons::Exit_Menu()
{
	AlienEngine::QuitApp();
}

