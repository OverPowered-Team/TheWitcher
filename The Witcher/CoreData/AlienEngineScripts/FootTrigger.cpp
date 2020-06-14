#include "PlayerController.h"
#include "SoundMaterial.h"
#include "FootTrigger.h"

FootTrigger::FootTrigger()
{
}

FootTrigger::~FootTrigger()
{
}

void FootTrigger::Start()
{
	if (player_go)
		player = player_go->GetComponent<PlayerController>();

	rbdy = GetComponent<ComponentRigidBody>();
}

void FootTrigger::Update()
{
	if(rbdy)
		rbdy->SetPosition(game_object->parent->transform->GetGlobalPosition());
}

void FootTrigger::OnTriggerEnter(ComponentCollider* collider)
{
	SoundMaterial* sound_mat = collider->game_object_attached->GetComponent<SoundMaterial>();
	if (sound_mat && player)
	{
		//player->OnTerrainEnter(sound_mat->GetMaterialName());
		player->audio->SetSwitchState("Material", sound_mat->GetMaterialName().c_str());
		player->audio->StartSound("Player_Footstep");
	}
}
