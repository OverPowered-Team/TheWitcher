#include "PlayerController.h"
#include "Enemy.h"
#include "PlayerProjectile.h"

PlayerProjectile::PlayerProjectile() : Alien()
{
}

PlayerProjectile::~PlayerProjectile()
{
}

void PlayerProjectile::Start()
{
	start_time = Time::GetGameTime();
	audio = GetComponent<ComponentAudioEmitter>();
}

void PlayerProjectile::Update()
{
	if ((Time::GetGameTime() - start_time) > duration)
		GameObject::Destroy(this->game_object);
	else
	{
		float3 pos = transform->GetGlobalPosition() + (direction * speed * Time::GetDT());
		transform->SetGlobalPosition(pos);
	}
}

void PlayerProjectile::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "Enemy") == 0) {

		Enemy* enemy = collider->game_object_attached->GetComponent<Enemy>();
		if (enemy && !enemy->IsDead())
		{
			float3 knock = direction * knock_back;
			float damage_dealt = enemy->GetDamaged(damage, player, knock);

			player->player_data.total_damage_dealt += damage_dealt;

			if (!enemy->IsDead())
			{
				if(audio)
					audio->StartSound("Play_RockThrow");
				GameObject::Destroy(this->game_object);
			}
				
		}
	}
	else if (strcmp(collider->game_object_attached->GetTag(), "Player") != 0)
	{
		if (audio)
			audio->StartSound("Play_RockThrow");
		GameObject::Destroy(this->game_object);
	}
}