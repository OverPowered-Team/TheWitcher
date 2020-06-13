#include "RockThrow.h"
#include "PlayerController.h"
#include "GameManager.h"
#include "PlayerManager.h"
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
	if (!throwable) {
		if (type == RockType::THROW)
			throw_direction = (GameManager::instance->player_manager->players[target]->transform->GetGlobalPosition() - transform->GetGlobalPosition()).Normalized();
		else {
			throw_direction = { 0, -4, 0 };
		}
		throwable = true;
	}

	switch (type)
	{
	case RockThrow::RockType::NONE:
		break;
	case RockThrow::RockType::THROW:
		game_object->transform->AddRotation(throw_rotation);
		transform->AddPosition(throw_direction * throw_speed * Time::GetDT());
		break;
	case RockThrow::RockType::FALL:
		game_object->transform->AddRotation(throw_rotation);
		transform->AddPosition(throw_direction * throw_speed * Time::GetDT());
		break;
	default:
		break;
	}

	game_object->GetComponent<ComponentRigidBody>()->SetPosition(transform->GetGlobalPosition());
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
					collided = true;
				}
			}

			if (strcmp((*it)->game_object_attached->GetTag(), "Enemy") == 0) {
				Enemy* enemy = (*it)->game_object_attached->GetComponent<Enemy>();
				if (enemy && enemy->game_object->GetComponent<Boss>()->state != Boss::BossState::DEAD && strcmp(enemy->game_object->GetName(), "Ciri") != 0) {
					float3 dir = ((*it)->game_object_attached->transform->GetGlobalPosition() - this->transform->GetGlobalPosition()).Normalized();
					dir.y = 0;
					enemy->GetDamaged(10, dir * 20);
					collided = true;
				}
			}
		}
		if (!particle_instance) {
			particle_instance = GameManager::instance->particle_pool->GetInstance("Ciri_Rock_Particle", float3::zero(), float3::zero(), game_object, true);
		}
	}

}
