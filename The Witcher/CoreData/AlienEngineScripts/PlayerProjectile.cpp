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
}

void PlayerProjectile::Update()
{
	if ((Time::GetGameTime() - start_time) > duration)
		GameObject::Destroy(game_object);
	else
	{
		float3 pos = transform->GetGlobalPosition() + (direction * speed * Time::GetDT());
		//float3 scale = transform->GetGlobalScale() + (float3(1,1,1) * speed * 0.05f * Time::GetDT());
		transform->SetGlobalPosition(pos);
		//transform->SetLocalScale(scale.Clamp(float3(0,0,0), float3(1.0f, 1.0f, 1.0f)));*/
	}
}

void PlayerProjectile::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "Enemy") == 0) {

		auto comps = collider->game_object_attached->GetComponents<Alien>();

		for (auto i = comps.begin(); i != comps.end(); ++i) {
			Enemy* enemy = dynamic_cast<Enemy*>(*i);
			if (enemy) {
				float3 knock = direction * knock_back;

				enemy->GetDamaged(damage, player, knock);
				if(!enemy->IsDead())
					Destroy(this->game_object);

				return;
			}
		}
	}
	else if (strcmp(collider->game_object_attached->GetTag(), "Player") != 0)
	{
		GameObject::Destroy(game_object);
	}
}