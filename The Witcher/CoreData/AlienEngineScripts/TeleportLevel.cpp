#include "TeleportLevel.h"

TeleportLevel::TeleportLevel() : Alien()
{
}

TeleportLevel::~TeleportLevel()
{
}

void TeleportLevel::Start()
{
	auto trans = teleportPrefab.ConvertToGameObject(float3::zero())->GetComponentsInChildren<ComponentTransform>();

	for (auto i = trans.begin(); i != trans.end(); ++i) {
		teleport_pos.push_back((*i)->GetGlobalPosition());
		teleport_rot.push_back((*i)->GetGlobalRotation());
	}
}

void TeleportLevel::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_RIGHT) && (current_pos < (teleport_pos.size() - 1)) /*&& godMode*/)
	{
		current_pos++;
		transform->SetGlobalPosition(teleport_pos[current_pos]);
		transform->SetGlobalRotation(teleport_rot[current_pos]);
	}
	else if (Input::GetKeyDown(SDL_SCANCODE_LEFT) && (current_pos > 0)/*&& godMode*/)
	{
		current_pos--;
		transform->SetGlobalPosition(teleport_pos[current_pos]);
		transform->SetGlobalRotation(teleport_rot[current_pos]);
	}

}