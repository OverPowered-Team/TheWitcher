#include "AttackTrigger.h"
#include "SoundMaterial.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"
#include "Enemy.h"

AttackTrigger::AttackTrigger()
{

}

AttackTrigger::~AttackTrigger()
{
}

void AttackTrigger::Update()
{
	if (rbdy)
	{
		rbdy->SetPosition(game_object->parent->transform->GetGlobalPosition());
	}
		
}

void AttackTrigger::OnTriggerEnter(ComponentCollider* collider)
{
	if (!player)
	{
		LOG("Error: No Player found on AttackTrigger");
		return;
	}

	if (strcmp(collider->game_object_attached->GetTag(), "Enemy") == 0)
	{
		Enemy* enemy = collider->game_object_attached->GetComponent<Enemy>();
		if(enemy && !enemy->IsDead() && !enemy->is_immune)
		{
			if (player->attacks->GetCurrentAttack() && player->attacks->GetCurrentAttack()->CanHit(enemy))
			{
				float damage = player->attacks->GetCurrentDMG();
				float3 knock = enemy->can_get_interrupted ? player->attacks->GetKnockBack(enemy->transform) : float3::zero();

				float damage_dealt = enemy->GetDamaged(damage, player, knock);
				player->OnHit(enemy, damage_dealt);
			}
		}
	}
	//Here we will be able to get the audio material and play the sound of the surface we hit
	SoundMaterial* s_material = collider->game_object_attached->GetComponent<SoundMaterial>();
	if (s_material)
	{
		player->audio->SetSwitchState("Material", s_material->GetMaterialName().c_str());
		player->audio->StartSound("Play_Impact");
	}

}

void AttackTrigger::Start()
{
	if (!player_obj)
	{
		LOG("Error: No Player found on AttackTrigger");
		return;
	}

	if (player_obj)
		player = player_obj->GetComponent<PlayerController>();

	rbdy = GetComponent<ComponentRigidBody>();
}
