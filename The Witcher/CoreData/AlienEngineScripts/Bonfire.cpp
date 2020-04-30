#include "Bonfire.h"

Bonfire::Bonfire() : Alien()
{
}

Bonfire::~Bonfire()
{
}

void Bonfire::Start()
{
}

void Bonfire::Update()
{
}

void Bonfire::OnDrawGizmos()
{
	Gizmos::DrawWireSphere(game_object->transform->GetLocalPosition(), bonfire_radius, Color::Orange());
}
