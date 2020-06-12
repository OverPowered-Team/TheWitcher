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
	/*std::vector<ComponentCollider*> colliders = Physics::OverlapSphere(transform->GetGlobalPosition(), 0.5);
	for (auto i = colliders.begin(); i != colliders.end(); ++i)
	{
		SoundMaterial* sound_mat = (*i)->game_object_attached->GetComponent<SoundMaterial>();
		if (sound_mat && player)
		{
			//player->OnTerrainEnter(sound_mat->GetMaterialName());
			player->audio->SetSwitchState("Material", sound_mat->GetMaterialName().c_str());
			player->audio->StartSound("Player_Footstep");
		}
	}*/
	rbdy->SetPosition(game_object->parent->transform->GetGlobalPosition());
}

void FootTrigger::OnTriggerEnter(ComponentCollider* collider)
{
	SoundMaterial* sound_mat = collider->game_object_attached->GetComponent<SoundMaterial>();
	if (sound_mat && player)
	{
		LOG("TOUCH GROUND");
		//player->OnTerrainEnter(sound_mat->GetMaterialName());
		player->audio->SetSwitchState("Material", sound_mat->GetMaterialName().c_str());
		player->audio->StartSound("Player_Footstep");
	}
}
