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

    case GhoulState::HIT:
    {
        velocity += velocity * knock_slow * Time::GetDT();
        velocity.y += gravity * Time::GetDT();
        character_ctrl->Move(velocity * Time::GetDT());
    }
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

    case GhoulState::DYING:
    {
		Dying(); 
        break;
    }
    }
}
