#include "GhoulMini.h"

GhoulMini::GhoulMini() : Ghoul()
{
}

GhoulMini::~GhoulMini()
{
}

void GhoulMini::StartEnemy()
{
    is_mini = true;

    Ghoul::StartEnemy();
}

void GhoulMini::UpdateEnemy()
{
    Enemy::UpdateEnemy();

    switch (state)
    {
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