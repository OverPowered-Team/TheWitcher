#include "PlayerController.h"

PlayerController::PlayerController() : Alien()
{
}

PlayerController::~PlayerController()
{
}

void PlayerController::Start()
{
	animator = (ComponentAnimator*)GetComponent(ComponentType::ANIMATOR);
}

void PlayerController::Update()
{
	playerData.speed = 0;


	if (Input::GetKeyRepeat(SDL_SCANCODE_W)) {
		playerData.speed += (100 * Time::GetDT());
	}

	if (Input::GetKeyDown(SDL_SCANCODE_E)) {
		playerData.attacking = true;
	}

	if (Input::GetKeyDown(SDL_SCANCODE_A)) {
		playerData.attacking = false;
	}

	transform->SetLocalPosition({ transform->GetLocalPosition().x, transform->GetLocalPosition().y, transform->GetLocalPosition().z + playerData.speed });
	animator->SetFloat("speed", playerData.speed);
	animator->SetBool("attacking", playerData.attacking);
}