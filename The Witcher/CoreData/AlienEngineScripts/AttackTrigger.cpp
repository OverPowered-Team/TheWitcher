#include "AttackTrigger.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"
#include "Enemy.h"

AttackTrigger::AttackTrigger()
{

}

AttackTrigger::~AttackTrigger()
{
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
		if(enemy && !enemy->IsDead() && player->attacks->GetCurrentAttack()->CanHit(enemy))
		{
			LOG("BANG BANG");
			float damage = player->attacks->GetCurrentDMG();
			float3 knock = enemy->can_get_interrupted ? player->attacks->GetKnockBack(transform) : float3::zero();

			float damage_dealt = enemy->GetDamaged(damage, player, knock);
			player->OnHit(enemy, damage_dealt);
		}
	}
	//Here we will be able to get the audio material and play the sound of the surface we hit
}

void AttackTrigger::Start()
{
	if (!player_obj)
	{
		LOG("Error: No Player found on AttackTrigger");
		return;
	}
	player = player_obj->GetComponent<PlayerController>();
}
