#include "UltiBar.h"
#include "../../Alien Engine/ComponentBar.h"

UltiBar::UltiBar() : Alien()
{
}

UltiBar::~UltiBar()
{
}

void UltiBar::Start()
{
	ultibar = game_object->GetChild("Ultibar");
	ultibar_charged = game_object->GetChild("Ultibar_Charged");
	ultibar->SetEnable(true);
	normal_ulti = ultibar->GetComponent<ComponentBar>();
	normal_ulti->SetBarValue(0);
	ultibar_charged->SetEnable(false);
}

void UltiBar::UpdateBar(float actual_value)
{
	normal_ulti->SetBarValue(actual_value);
	if (actual_value == 0)
	{
		ultibar->SetEnable(true);
		ultibar_charged->SetEnable(false);
	}
}

void UltiBar::MaxBar()
{
	normal_ulti->SetBarValue(1);
	ultibar->SetEnable(false);
	ultibar_charged->SetEnable(true);
}