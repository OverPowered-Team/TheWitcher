#include "CameraWagonette.h"

CameraWagonette::CameraWagonette() : Alien()
{
}

CameraWagonette::~CameraWagonette()
{
}

void CameraWagonette::Start()
{
	minecart = game_object->parent->GetChild("Vagonete_with_panas");
	offset = transform->GetGlobalPosition() - minecart->transform->GetGlobalPosition();
}

void CameraWagonette::Update()
{
	transform->SetGlobalPosition(minecart->transform->GetGlobalPosition() + offset);
}