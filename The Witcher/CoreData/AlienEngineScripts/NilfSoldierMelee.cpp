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
		if (distance < stats["VisionRange"].GetValue())
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
		}
		break;

	case NilfgaardSoldierState::HIT:
	{
		velocity += velocity * knock_slow * Time::GetDT();
		character_ctrl->Move(velocity * Time::GetDT());
	}
	break;

	case NilfgaardSoldierState::DYING:
	{
		EnemyManager* enemy_manager = GameObject::FindWithName("GameManager")->GetComponent<EnemyManager>();
		Invoke([enemy_manager, this]() -> void {enemy_manager->DeleteEnemy(this); }, 5);
		animator->PlayState("Death");
		audio_emitter->StartSound("SoldierDeath");
		last_player_hit->OnEnemyKill();
		state = NilfgaardSoldierState::DEAD;
		if (m_controller && is_combat)
		{
			is_combat = false;
			m_controller->EnemyLostSight((Enemy*)this);
		}
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