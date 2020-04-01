#include "Win_lose_menu.h"

Win_lose_menu::Win_lose_menu() : Alien()
{
}

Win_lose_menu::~Win_lose_menu()
{
}

void Win_lose_menu::Start()
{
	if (!have_won)
	{
		win_panel->SetEnable(false);
		win_icon->SetEnable(false);
		defeat_icon->SetEnable(true);
		defeat_panel->SetEnable(true);
	}
	else
	{
		defeat_icon->SetEnable(false);
		defeat_panel->SetEnable(false);
		win_panel->SetEnable(true);
		win_icon->SetEnable(true);
	}
}
