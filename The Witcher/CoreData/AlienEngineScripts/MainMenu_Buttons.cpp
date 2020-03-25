#include "MainMenu_Buttons.h"

MainMenu_Buttons::MainMenu_Buttons() : Alien()
{
}

MainMenu_Buttons::~MainMenu_Buttons()
{
}

void MainMenu_Buttons::NewGame()
{
	//SceneManager::LoadScene("newTRIGGER");
}

void MainMenu_Buttons::ExitGame()
{
	AlienEngine::QuitApp();
}

void MainMenu_Buttons::Controls()
{
}

void MainMenu_Buttons::Settings()
{
}

void MainMenu_Buttons::Muffin()
{
	AlienEngine::OpenURL("https://github.com/OverPowered-Team");
}


