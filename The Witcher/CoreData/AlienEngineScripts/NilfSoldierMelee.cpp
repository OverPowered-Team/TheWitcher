#include "NilfSoldierMelee.h"
#include "PlayerController.h"
#include "EnemyManager.h"
#include "MusicController.h"

NilfSoldierMelee::NilfSoldierMelee() : NilfgaardSoldier()
{
}

NilfSoldierMelee::~NilfSoldierMelee()
{
}

void NilfSoldierMelee::UpdateEnemy()
{
	Enemy::UpdateEnemy();

	switch (state)
	{
	case NilfgaardSoldierState::IDLE:
		if (distance < stats["VisionRange"].GetValue() || is_obstacle)
			SetState("Move");
		break;

	case NilfgaardSoldierState::MOVE:
		Move(direction);
		break;

	case NilfgaardSoldierState::GUARD:
	{
		//Tiene que estar animacion de guardia
		Guard();
	}
		break;

	case NilfgaardSoldierState::STUNNED:
		if (Time::GetGameTime() - current_stun_time > stun_time)
		{
			SetState("Idle");
			animator->PlayState("Idle");
			animator->SetBool("stunned", false);
		}
		break;

	case NilfgaardSoldierState::HIT:
	{
		velocity += velocity * knock_slow * Time::GetDT();
		velocity.y += gravity * Time::GetDT();
		character_ctrl->Move(velocity * Time::GetDT());
	}
	break;

	case NilfgaardSoldierState::DYING:
	{
		animator->PlayState("Death");
		last_player_hit->OnEnemyKill((uint)type);
		state = NilfgaardSoldierState::DEAD;
		if (m_controller && is_combat)
		{
			is_combat = false;
			m_controller->EnemyLostSight((Enemy*)this);
		}
		if (is_obstacle)
		{
			BlockerObstacle* blocker = game_object->parent->parent->GetComponent<BlockerObstacle>();
			if (blocker)
				blocker->ReleaseMyself(this);
			else
				LOG("There's no blocker");
		}
		break;

	case NilfgaardSoldierState::DEAD:
		IDontWannaGoMrStark();
		break;
	}
	}
}

void NilfSoldierMelee::Action()
{
	SetState("Attack");
	animator->PlayState("Attack");
	animator->SetCurrentStateSpeed(stats["AttackSpeed"].GetValue());
}