#include "RockThrow.h"
#include "PlayerController.h"
#include "GameManager.h"
#include "ParticlePool.h"
#include "Enemy.h"
#include "Boss.h"

RockThrow::RockThrow() : Alien()
{
}

RockThrow::~RockThrow()
{
}

void RockThrow::Start()
{
}

void RockThrow::Update()
{
	if (timer < lifetime)
		timer += Time::GetDT();
	else {
		Destroy(game_object);
	}
}

void RockThrow::ReleaseExplosionParticle()
{
	if(particle_instance)
		GameManager::instance->particle_pool->ReleaseInstance("Ciri_Rock_Particle", particle_instance);
	Destroy(game_object);
}

void RockThrow::OnTriggerEnter(ComponentCollider* collider)
{
	if (!collided) {
		std::vector<ComponentCollider*> hitted;
		hitted = Physics::OverlapSphere(game_object->transform->GetGlobalPosition(), 5);
		for (auto it = hitted.begin(); it != hitted.end(); ++it) {

			if (strcmp((*it)->game_object_attached->GetTag(), "Player") == 0) {
				PlayerController* player_ctrl = (*it)->game_object_attached->GetComponent<PlayerController>();
				if (player_ctrl && !player_ctrl->is_immune && player_ctrl->state->type != StateType::DEAD) {
					float3 dir = ((*it)->game_object_attached->transform->GetGlobalPosition() - this->transform->GetGlobalPosition()).Normalized();
					dir.y = 0;
					player_ctrl->ReceiveDamage(10.0f, dir * 20);
				}
			}

			if (strcmp((*it)->game_object_attached->GetTag(), "Enemy") == 0) {
				Enemy* enemy = (*it)->game_object_attached->GetComponent<Enemy>();
				if (enemy && enemy->game_object->GetComponent<Boss>()->state != Boss::BossState::DEAD) {
					float3 dir = ((*it)->game_object_attached->transform->GetGlobalPosition() - this->transform->GetGlobalPosition()).Normalized();
					dir.y = 0;
					enemy->GetDamaged(10, dir * 20);
				}
			}

		}
		game_object->GetComponent<ComponentMesh>()->SetEnable(false);
		game_object->GetComponent<ComponentSphereCollider>()->SetEnable(false);

		if (!particle_instance) {
			particle_instance = GameManager::instance->particle_pool->GetInstance("Ciri_Rock_Particle", float3::zero(), float3::zero(), game_object, true);
			Invoke(std::bind(&RockThrow::ReleaseExplosionParticle, this), 0.6f);
		}

		collided = true;
	}

}
