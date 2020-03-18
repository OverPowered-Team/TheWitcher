#include "PlayerTest.h"

PlayerTest::PlayerTest() : Alien()
{
}

PlayerTest::~PlayerTest()
{
}

void PlayerTest::Start()
{
}

void PlayerTest::Update()
{
	if (Input::GetKey(SDL_SCANCODE_W)) {
		transform->SetGlobalPosition(transform->GetGlobalPosition() + float3::unitZ() * speed * Time::GetDT());
	}
	if (Input::GetKey(SDL_SCANCODE_S)) {
		transform->SetGlobalPosition(transform->GetGlobalPosition() - float3::unitZ() * speed * Time::GetDT());
	}
	if (Input::GetKey(SDL_SCANCODE_A)) {
		transform->SetGlobalPosition(transform->GetGlobalPosition() + float3::unitX() * speed * Time::GetDT());
	}
	if (Input::GetKey(SDL_SCANCODE_D)) {
		transform->SetGlobalPosition(transform->GetGlobalPosition() - float3::unitX() * speed * Time::GetDT());
	}
}