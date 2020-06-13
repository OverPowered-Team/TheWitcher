#include "MainMenu_Buttons.h"
#include "Extra_Menus.h"

MainMenu_Buttons::MainMenu_Buttons() : Alien()
{
}

MainMenu_Buttons::~MainMenu_Buttons()
{
}

void MainMenu_Buttons::NewGame()
{
	SceneManager::LoadScene("Intro", FadeToBlackType::HORIZONTAL_CURTAIN);
}

void MainMenu_Buttons::ExitGame()
{
	AlienEngine::QuitApp();
}

void MainMenu_Buttons::Controls()
{
	GameObject::FindWithName("Extra_Menus")->SetEnable(true);
	GameObject::FindWithName("Extra_Menus")->GetComponent<Extra_Menus>()->MenuSpawn(Extra_Menus::MENU::CONTROLS);
	GameObject::FindWithName("Main_Menu_UI")->SetEnable(false);
}

void MainMenu_Buttons::Credits()
{
	SceneManager::LoadScene("CreditsMenu", FadeToBlackType::FADE);
}


void MainMenu_Buttons::Muffin()
{
	AlienEngine::OpenURL("https://github.com/OverPowered-Team");
}

void MainMenu_Buttons::AddVolume()
{
	ComponentAudioEmitter* emitter = GetComponent<ComponentAudioEmitter>();
	//UIConfig::AddVolume(emitter);

}

void MainMenu_Buttons::SubtractVolume()
{
	ComponentAudioEmitter* emitter = GetComponent<ComponentAudioEmitter>();
	//UIConfig::SubtractVolume(emitter);
}

void MainMenu_Buttons::FullScreen()
{
	if (Screen::IsFullScreen())
		Screen::SetFullScreen(false);
	else
		Screen::SetFullScreen(true);
	//UIConfig::FullScreen();
}


