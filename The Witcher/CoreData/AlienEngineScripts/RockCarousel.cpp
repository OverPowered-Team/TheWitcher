#include "RockCarousel.h"

RockCarousel::RockCarousel() : Alien()
{
}

RockCarousel::~RockCarousel()
{
}

void RockCarousel::Start()
{
}

void RockCarousel::Update()
{
	game_object->transform->AddRotation({ 0.0f, 10.0f, 0.0f });
}