#include "Testanimator.h"

Testanimator::Testanimator() : Alien()
{
}

Testanimator::~Testanimator()
{
}

void Testanimator::Start()
{
	anim = (ComponentAnimator*)GetComponent(ComponentType::ANIMATOR);
}

void Testanimator::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_W))
		anim->SetBool("NewBool", true);
}