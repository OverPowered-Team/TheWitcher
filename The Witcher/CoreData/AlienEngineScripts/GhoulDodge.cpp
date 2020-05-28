#include "GhoulDodge.h"
#include "EnemyManager.h"
#include "PlayerController.h"
#include "MusicController.h"

GhoulDodge::GhoulDodge() : Ghoul()
{
}

GhoulDodge::~GhoulDodge()
{
}

void GhoulDodge::UpdateEnemy()
{
    Enemy::UpdateEnemy();

    float angle = atan2f(direction.z, direction.x);
    Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());
    transform->SetGlobalRotation(rot);

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

    case GhoulState::JUMP:
        JumpImpulse();
        break;

    case GhoulState::STUNNED:
        if (Time::GetGameTime() - current_stun_time > stun_time)
        {
            state = GhoulState::IDLE;
        }
        break;

    case GhoulState::ATTACK:
        if (player_controllers[current_player]->state->type == StateType::ATTACKING && rand_num == 0)
        {
            state = GhoulState::DODGE;
            animator->PlayState("Jump");
        }
        break;
    case GhoulState::HIT:
        velocity += velocity * knock_slow * Time::GetDT();
        character_ctrl->Move(velocity * Time::GetDT());
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
    if (distance < stats["JumpRange"].GetValue())
    {
        float3 jump_direction = -direction * stats["Agility"].GetValue() * stats["JumpForce"].GetValue();
        character_ctrl->Move(jump_direction * Time::GetDT() * Time::GetScaleTime());
    }
}

void GhoulDodge::OnAnimationEnd(const char* name)
{
    if (strcmp(name, "Slash") == 0) {
        can_get_interrupted = true;
        if (distance < stats["VisionRange"].GetValue() && distance > stats["JumpRange"].GetValue())
        {
            state = GhoulState::MOVE;
        }
        else
        {
            state = GhoulState::IDLE;
        }
        rand_num = Random::GetRandomIntBetweenTwo(0, 2);
    }
    else if (strcmp(name, "Jump") == 0)
    {
        if (distance < stats["VisionRange"].GetValue())
            state = GhoulState::MOVE;
        else
            state = GhoulState::IDLE;

        rand_num = Random::GetRandomIntBetweenTwo(0, 2);
    }
    else if (strcmp(name, "Hit") == 0)
    {
        state = GhoulState::IDLE;
        rand_num = Random::GetRandomIntBetweenTwo(0, 2);
    }
}

