#include "TeleportLevel.h"

TeleportLevel::TeleportLevel() : Alien()
{
}

TeleportLevel::~TeleportLevel()
{
}

void TeleportLevel::Start()
{
	ComponentTransform** trans = nullptr;
	uint size = teleportPrefab.ConvertToGameObject(float3::zero())->GetComponentsInChildren(ComponentType::TRANSFORM, (Component***)&trans, false);

	for (uint i = 0u; i < size; ++i) {
		teleport_pos.push_back(trans[i]->GetGlobalPosition());
		teleport_rot.push_back(trans[i]->GetGlobalRotation());
	}

	GameObject::FreeArrayMemory((void***)&trans);
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