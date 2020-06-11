#include "GhoulDodge.h"
#include "EnemyManager.h"
#include "PlayerManager.h"
#include "PlayerController.h"
#include "MusicController.h"
#include "GameManager.h"

GhoulDodge::GhoulDodge() : Ghoul()
{
}

GhoulDodge::~GhoulDodge()
{
}

void GhoulDodge::UpdateEnemy()
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

    case GhoulState::HIT:
    {
        velocity += velocity * knock_slow * Time::GetDT();
        velocity.y += gravity * Time::GetDT();
        character_ctrl->Move(velocity * Time::GetDT());
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

    case GhoulState::ATTACK:
        if (CheckIfDodge())
        {
            state = GhoulState::DODGE;
            animator->PlayState("Dodge");
        }

        if (distance < stats["AttackRange"].GetValue())
            animator->SetBool("attack", true);
        else
            animator->SetBool("attack", false);

        break;

    case GhoulState::DODGE:
        Dodge();
        break;

    case GhoulState::DYING:
    {
		Dying(); 

        break;
    }
    }
}

void GhoulDodge::Dodge()
{
    float3 jump_direction = -direction.Normalized() * stats["Agility"].GetValue() * stats["JumpForce"].GetValue();
    character_ctrl->Move(jump_direction * Time::GetDT() * Time::GetScaleTime());
}

bool GhoulDodge::CheckIfDodge()
{
    bool ret = false;

    if (rand_num == 0)
    {
        int other_player = (current_player == 0) ? 1 : 0;
        if (player_controllers[current_player]->state->type == StateType::ATTACKING)
        {
            ret = true;
        }
        else if (player_controllers[other_player]->state->type == StateType::ATTACKING && player_controllers[other_player]->transform->GetGlobalPosition().Distance(game_object->transform->GetGlobalPosition()) < 2.0f)
        {
            ret = true;
        }
    }

    return ret;
}

void GhoulDodge::OnAnimationEnd(const char* name)
{
    if (strcmp(name, "Slash") == 0) {
        can_get_interrupted = true;
        ReleaseParticle("EnemyAttackParticle");
        if (distance < stats["VisionRange"].GetValue() && distance > stats["JumpRange"].GetValue())
            SetState("Move");
        else
            SetState("Idle");

        /*stats["HitSpeed"].SetCurrentStat(stats["HitSpeed"].GetBaseValue());*/
        //animator->SetCurrentStateSpeed(stats["HitSpeed"].GetValue());
        rand_num = Random::GetRandomIntBetweenTwo(0, 2);
        animator->SetBool("attack", false);
    }
    else if (strcmp(name, "Jump") == 0 || strcmp(name, "Dodge") == 0)
    {
        if (distance < stats["AttackRange"].GetValue())
            SetState("Attack");
        else if (distance < stats["VisionRange"].GetValue() && distance > stats["MoveRange"].GetValue())
            SetState("Move");
        else
            SetState("Idle");

        ReleaseParticle("AreaAttackSlash");
        ReleaseParticle("AreaAttackSphere");
        ReleaseParticle("AreaAttackRock");

        can_jump = false;
        rand_num = Random::GetRandomIntBetweenTwo(0, 2);
    }
    else if (strcmp(name, "Hit") == 0)
    {
        ReleaseParticle("hit_particle");

        if (stats["Health"].GetValue() <= 0.0F)
        {
            if (!was_dizzy)
                was_dizzy = true;
            else
            {
                state = GhoulState::DYING;
                GameManager::instance->player_manager->IncreaseUltimateCharge(10);
            }
        }
        else if (is_attacking)
            ChangeAttackEnemy();
        else if (!is_dead)
            SetState("Idle");

        rand_num = Random::GetRandomIntBetweenTwo(0, 2);
    }
    else if ((strcmp(name, "Dizzy") == 0) && stats["Health"].GetValue() <= 0)
    {
        state = GhoulState::DYING;
        GameManager::instance->player_manager->IncreaseUltimateCharge(10);
    }
}

