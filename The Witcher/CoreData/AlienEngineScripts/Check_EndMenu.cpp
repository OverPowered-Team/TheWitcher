#include "Check_EndMenu.h"

Check_EndMenu::Check_EndMenu() : Alien()
{
}

Check_EndMenu::~Check_EndMenu()
{
}

void Check_EndMenu::Start()
{
}

void Check_EndMenu::Update()
{
		if (((Input::GetControllerButtonDown(1, Input::CONTROLLER_BUTTON_Y)
			|| Input::GetControllerButtonDown(2, Input::CONTROLLER_BUTTON_Y))))
		{
			SceneManager::LoadScene("Main_Menu");
		}
}