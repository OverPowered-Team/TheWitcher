#include "GhoulOriginal.h"
#include "PlayerController.h"
#include "EnemyManager.h"
#include "MusicController.h"

GhoulOriginal::GhoulOriginal() : Ghoul()
{
}

GhoulOriginal::~GhoulOriginal()
{
}

void GhoulOriginal::UpdateEnemy()
{
    Enemy::UpdateEnemy();

    switch (state)
    {
	case GhoulState::AWAKE: 
		DoAwake();
		break; 

    case GhoulState::IDLE:
        if (distance < stats["VisionRange"].GetValue() || is_obstacle)
            state = GhoulState::MOVE;
        break;

    case GhoulState::MOVE:
        Move(direction);
        break;

    //case GhoulState::GUARD:
    //    Guard();
    //    break;

    case GhoulState::HIT:
    {
        velocity += velocity * knock_slow * Time::GetDT();
        velocity.y += gravity * Time::GetDT();
        character_ctrl->Move(velocity * Time::GetDT());
    }
        break;

    case GhoulState::ATTACK:
    {
        if (distance < stats["AttackRange"].GetValue())
            animator->SetBool("attack", true);
        else
            animator->SetBool("attack", false);
    }
        break;

    case GhoulState::JUMP:
        JumpImpulse();
        if (distance < stats["AttackRange"].GetValue())
            animator->SetBool("attack", true);
        else
            animator->SetBool("attack", false);
        break;

    case GhoulState::STUNNED:
        if (Time::GetGameTime() - current_stun_time > stun_time)
        {
            state = GhoulState::IDLE;
        }
        break;

    case GhoulState::DYING:
    {
		Dying(); 
        break;
    }
    }
}
