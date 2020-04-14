#include "Enemy.h"
#include "EnemyManager.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"
#include "Effect.h"

void Enemy::Awake()
{
	((EnemyManager*)(GameObject::FindWithName("GameManager")->GetComponentScript("EnemyManager")))->AddEnemy(this);
	attack_collider = (ComponentCollider*)game_object->GetChild("EnemyAttack")->GetComponent(ComponentType::BOX_COLLIDER);
}

void Enemy::StartEnemy()
{
	animator = (ComponentAnimator*)GetComponent(ComponentType::ANIMATOR);
	character_ctrl = (ComponentCharacterController*)GetComponent(ComponentType::CHARACTER_CONTROLLER);
	state = EnemyState::IDLE;
	std::string json_str;

	character_ctrl->SetRotation(Quat::identity());

	switch (type)
	{
	case EnemyType::NONE:
		json_str = "None";
		break;
	case EnemyType::GHOUL:
		json_str = "ghoul";
		break;
	case EnemyType::NILFGAARD_SOLDIER:
		json_str = "nilfgaardsoldier";
		break;
	default:
		break;
	}

	SetStats(json_str.data());
}

void Enemy::UpdateEnemy()
{
	float distance_1 = player_controllers[0]->transform->GetGlobalPosition().DistanceSq(game_object->transform->GetLocalPosition());
	float3 direction_1 = player_controllers[0]->transform->GetGlobalPosition() - game_object->transform->GetGlobalPosition();

	float distance_2 = player_controllers[1]->transform->GetGlobalPosition().DistanceSq(game_object->transform->GetLocalPosition());
	float3 direction_2 = player_controllers[1]->transform->GetGlobalPosition() - game_object->transform->GetGlobalPosition();

	if (player_controllers[0]->state == PlayerController::PlayerState::DEAD)
	{
		distance = distance_2;
		direction = direction_2.Normalized();
	}
	else if (player_controllers[1]->state == PlayerController::PlayerState::DEAD)
	{
		distance = distance_1;
		direction = direction_1.Normalized();
	}
	else
	{
		distance = (distance_1 < distance_2) ? distance_1 : distance_2;
		direction = (distance_1 < distance_2) ? direction_1.Normalized() : direction_2.Normalized();
	}

	for (auto it = effects.begin(); it != effects.end(); )
	{
		if (!(*it)->UpdateEffect())
		{
			delete (*it);
			it = effects.erase(it);
			continue;
		}
		else if((*it)->ticks_time > 0)
		{
			if (particles[(*it)->name])
				particles[(*it)->name]->SetEnable(true);
			stats["Health"].ModifyCurrentStat((*it));
			stats["Agility"].ModifyCurrentStat((*it));
		}
		++it;
	}
}

void Enemy::CleanUpEnemy()
{
	delete animator;
	animator = nullptr;
	delete character_ctrl;
	character_ctrl = nullptr;

	for (auto it_pc = player_controllers.begin(); it_pc != player_controllers.end();)
	{
		delete (*it_pc);
		it_pc = player_controllers.erase(it_pc);
	}
	for (auto it_eff = effects.begin(); it_eff != effects.end();)
	{
		delete (*it_eff);
		it_eff = effects.erase(it_eff);
	}
}

void Enemy::SetStats(const char* json)
{
	std::string json_path = ENEMY_JSON + std::string(json) + std::string(".json");
	LOG("READING ENEMY STAT GAME JSON WITH NAME %s", json_path.data());

	JSONfilepack* stat = JSONfilepack::GetJSON(json_path.c_str());
	if (stat)
	{
		stats["Health"] = Stat( "Health", stat->GetNumber("Health"));
		stats["Agility"] = Stat("Agility", stat->GetNumber("Agility"));
		stats["Damage"] = Stat("Damage", stat->GetNumber("Damage"));
		stats["AttackSpeed"] = Stat("AttackSpeed", stat->GetNumber("AttackSpeed"));
		stats["AttackRange"] = Stat("AttackRange", stat->GetNumber("AttackRange"));
		stats["VisionRange"] = Stat("VisionRange", stat->GetNumber("VisionRange"));
	}

	JSONfilepack::FreeJSON(stat);
}

void Enemy::Move(float3 direction)
{
	character_ctrl->SetWalkDirection(direction * stats["Agility"].GetValue());
	animator->SetFloat("speed", stats["Agility"].GetValue());

	float angle = atan2f(direction.z, direction.x);
	Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());
	character_ctrl->SetRotation(rot);

	if (distance < stats["AttackRange"].GetValue())
	{
		state = Enemy::EnemyState::ATTACK;
		character_ctrl->SetWalkDirection(float3(0.0F, 0.0F, 0.0F));
		animator->SetFloat("speed", 0.0F);
		Attack();
	}

	if (distance > stats["VisionRange"].GetValue())
	{
		state = Enemy::EnemyState::IDLE;
		character_ctrl->SetWalkDirection(float3(0.0F, 0.0F, 0.0F));
		animator->SetFloat("speed", 0.0F);
	}
}

void Enemy::ActivateCollider()
{
	if (attack_collider)
	{
		attack_collider->SetEnable(true);
	}
}

void Enemy::DeactivateCollider()
{
	if (attack_collider)
	{
		attack_collider->SetEnable(false);
	}
}

void Enemy::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "PlayerAttack") == 0 && state != EnemyState::DEAD) {
		PlayerController* player = static_cast<PlayerController*>(collider->game_object_attached->GetComponentScriptInParent("PlayerController"));
		if (player)
		{
			float dmg_received = player->attacks->GetCurrentDMG();
			player->OnHit(this, GetDamaged(dmg_received));

			if (state == EnemyState::DYING)
				player->OnEnemyKill();
		}
	}
}

float Enemy::GetDamaged(float dmg)
{
	float aux_health = stats["Health"].GetValue();
	stats["Health"].DecreaseStat(dmg);

	if (stats["Health"].GetValue() == 0.0F) {
		state = EnemyState::DYING;
		animator->PlayState("Death");
	}
	else
	{
		state = EnemyState::HIT;
		animator->PlayState("Hit");
		character_ctrl->SetWalkDirection(float3::zero());
	}

	return aux_health - stats["Health"].GetValue();
}

void Enemy::AddEffect(Effect* new_effect)
{
	effects.push_back(new_effect);

	for (auto it = stats.begin(); it != stats.end(); ++it)
	{
		if (new_effect->AffectsStat(it->second.name) && new_effect->ticks_time == 0)
			it->second.ApplyEffect(new_effect);
	}
}
