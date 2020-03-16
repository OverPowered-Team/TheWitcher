#include "TestSwitches.h"

TestSwitches::TestSwitches() : Alien()
{
}

TestSwitches::~TestSwitches()
{
}

void TestSwitches::Start()
{
	emmitter = (ComponentAudioEmitter*)GetComponent(ComponentType::A_EMITTER);
}

void TestSwitches::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_U))
	{
		emmitter->StartSound();
	}
	if (Input::GetKeyDown(SDL_SCANCODE_T))
	{
		emmitter->SetSwitchState("Material", "Dirt");
	}
	if (Input::GetKeyDown(SDL_SCANCODE_Y))
	{
		emmitter->SetSwitchState("Material", "Sand");
	}
}
