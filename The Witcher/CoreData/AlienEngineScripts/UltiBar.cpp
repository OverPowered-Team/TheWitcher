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
	ultibar->SetEnable(true);
	normal_ulti = (ComponentBar*)ultibar->GetComponent(ComponentType::UI_BAR);
	normal_ulti->SetBarValue(0);
	ultibar_charged->SetEnable(false);
}

void UltiBar::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_7))
	{
		normal_ulti->SetBarValue(normal_ulti->GetBarValue() + 0.1f);
	}

	if (normal_ulti->GetBarValue() >= 1)
	{
		normal_ulti->SetBarValue(1);
		ultibar->SetEnable(false);
		ultibar_charged->SetEnable(true);
	}

	if (Input::GetKeyDown(SDL_SCANCODE_0))
	{
		ultibar->SetEnable(true);
		normal_ulti->SetBarValue(0);
		ultibar_charged->SetEnable(false);
	}

}