#include "DrownedRange.h"
#include "EnemyManager.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"
#include "ArrowScript.h"

DrownedRange::DrownedRange() : Drowned()
{
}

DrownedRange::~DrownedRange()
{
}

void DrownedRange::UpdateEnemy()
{
	Enemy::UpdateEnemy();
	switch (state)
	{
	case DrownedState::IDLE:
		if (distance < stats["AttackRange"].GetValue() && distance > stats["HideDistance"].GetValue())
		{
			animator->SetFloat("speed", 0.0F);
			character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
			animator->PlayState("GetOff");
			state = DrownedState::GETOFF;
			is_hide = false;
		}
		break;

	case DrownedState::GETOFF:
		if (transform->GetGlobalScale().y < 0.006)
			transform->AddScale(float3(0.0f, 0.0001f, 0.0f));
		else
		{
			state = DrownedState::ATTACK;
			animator->PlayState("Attack");
		}
		break;

	case DrownedState::ATTACK:
		if (distance < stats["HideDistance"].GetValue() /*|| distance > stats["AttackRange"].GetValue()*/)
		{
			state = DrownedState::HIDE;
			current_hide_time = Time::GetGameTime();
		}
		break;

	case DrownedState::HIDE:
		if (Time::GetGameTime() - current_hide_time > max_hide_time)
		{
			animator->PlayState("Hide");
			transform->AddScale(float3(0.0f, -0.005f, 0.0f));
			state = DrownedState::IDLE;
			is_hide = true;
		}
		break;

	case DrownedState::DYING:
	{
		EnemyManager* enemy_manager = GameObject::FindWithName("GameManager")->GetComponent< EnemyManager>();
		Invoke([enemy_manager, this]() -> void {enemy_manager->DeleteEnemy(this); }, 5);
		state = DrownedState::DEAD;
		animator->PlayState("Dead");
		//audio_emitter->StartSound("DrownedDeath");
	}
	}
}


void DrownedRange::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "PlayerAttack") == 0 && state != DrownedState::DEAD) {

		if (!is_hide)
		{
			PlayerController* player = collider->game_object_attached->GetComponentInParent<PlayerController>();
			if (player && player->attacks->GetCurrentAttack()->CanHit(this))
			{
				float dmg_received = player->attacks->GetCurrentDMG();
				player->OnHit(this, GetDamaged(dmg_received, player));

				if (state == DrownedState::DYING)
					player->OnEnemyKill();

				HitFreeze(player->attacks->GetCurrentAttack()->info.freeze_time);
			}
		}
	}
}

void DrownedRange::ShootSlime()
{
	float3 slime_pos = transform->GetGlobalPosition() + direction.Mul(1).Normalized() + float3(0.0F, 1.0F, 0.0F);
	GameObject* arrow_go = GameObject::Instantiate(slime, slime_pos);
	ComponentRigidBody* arrow_rb = arrow_go->GetComponent<ComponentRigidBody>();
	audio_emitter->StartSound("SoldierShoot");
	arrow_go->GetComponent<ArrowScript>()->damage = stats["Damage"].GetValue();
	arrow_rb->SetRotation(RotateProjectile());
	arrow_rb->AddForce(direction.Mul(20), ForceMode::IMPULSE);
}
