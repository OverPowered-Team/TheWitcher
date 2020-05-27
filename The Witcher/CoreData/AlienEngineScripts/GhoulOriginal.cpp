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
        if (distance < stats["VisionRange"].GetValue())
            state = GhoulState::MOVE;
        break;

    case GhoulState::MOVE:
        Move(direction);
        break;

    case GhoulState::HIT:
        velocity += velocity * knock_slow * Time::GetDT();
        character_ctrl->Move(velocity * Time::GetDT());
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
        EnemyManager* enemy_manager = GameObject::FindWithName("GameManager")->GetComponent< EnemyManager>();
        //Ori Ori function sintaxis
        Invoke([enemy_manager, this]() -> void {enemy_manager->DeleteEnemy(this); }, 5);
        animator->PlayState("Death");
        audio_emitter->StartSound("GhoulDeath");
        last_player_hit->OnEnemyKill();
        state = GhoulState::DEAD;
        if (m_controller && is_combat)
        {
            is_combat = false;
            m_controller->EnemyLostSight((Enemy*)this);
        }
        break;
    }
    }
}

void GhoulOriginal::DoAwake()
{
	if (distance < stats["VisionRange"].GetValue())
	{
		state = GhoulState::MOVE;
		return; 
	}

	float3 current_position = game_object->transform->GetGlobalPosition();

	switch (awake_behaviour)
	{
	case AwakeBehaviour::FOLLOW_CURVE:
	{
		Curve& curve = awake_curve->GetComponent<ComponentCurve>()->curve;

		current_curve_point += curve_speed * Time::GetScaleTime() * Time::GetDT();
		if (current_curve_point >= 1.f)
		{
			LOG("Enemy has completed a curve cycle!!");
			current_curve_point = 0.f;
			current_curve_point += curve_speed * Time::GetScaleTime() * Time::GetDT();
		}

		float3 next_position = curve.ValueAt(current_curve_point);
		next_position.y = current_position.y;
		float3 direction = (next_position - current_position).Normalized();

		Move(direction);

		break;

	}
		
	case AwakeBehaviour::WANDER:
	{
		if (wander_rest) // Resting
		{

			if ((current_wander_time += Time::GetDT()) >= wander_rest_time)
			{
				LOG("Enemy resumed wander!"); 

				wander_rest = false; 
				current_wander_time = 0.0f;
				float delta = (float)hypot(wander_radius, wander_radius);
				float deltaX = Random::GetRandomFloatBetweenTwo(-delta, delta);
				float deltaZ = Random::GetRandomFloatBetweenTwo(-delta, delta);
				lastWanderTargetPos = float3(start_pos.x + deltaX, start_pos.y, start_pos.z + deltaZ);

			}
		}
		else // Going to next position
		{
			Move((lastWanderTargetPos - current_position).Normalized());

			if ((lastWanderTargetPos - current_position).Length() <= wander_precision) // Arrived to next position
			{
				LOG("Enemy arrived to wander rest position!");
				wander_rest = true;
				character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
				animator->SetFloat("speed", 0.0F);
			}
				
		}

		break;
	}
	
	}


}