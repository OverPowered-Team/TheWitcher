#include "DrownedRange.h"
#include "EnemyManager.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"

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
		if (distance < stats["AttackRange"].GetValue())
		{
			animator->SetFloat("speed", 0.0F);
			character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
			animator->PlayState("GetOff");
			state = DrownedState::ATTACK;
			is_hide = false;
		}
		break;

	case DrownedState::ATTACK:
		if (distance < stats["HideDistance"].GetValue())
		{
			state = DrownedState::HIDE;
			current_hide_time = Time::GetGameTime();
		}
		break;

	case DrownedState::HIDE:
		if (Time::GetGameTime() - current_hide_time > max_hide_time)
		{
			animator->PlayState("Hide");
			state = DrownedState::IDLE;
			is_hide = true;
		}
		break;

	case DrownedState::DYING:
	{
		EnemyManager* enemy_manager = GameObject::FindWithName("GameManager")->GetComponent< EnemyManager>();
		Invoke([enemy_manager, this]() -> void {enemy_manager->DeleteEnemy(this); }, 5);
		state = DrownedState::DEAD;
		last_player_hit->OnEnemyKill();
		//audio_emitter->StartSound("DrownedDeath");
	}
	}
}

void DrownedRange::OnAnimationEnd(const char* name)
{
	if (strcmp(name, "GetOff") == 0)
	{
		state = DrownedState::ATTACK;
		animator->PlayState("Attack");
	}
	else if (strcmp(name, "Attack") == 0)
	{
		state = DrownedState::ATTACK;
		animator->PlayState("Attack");
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
