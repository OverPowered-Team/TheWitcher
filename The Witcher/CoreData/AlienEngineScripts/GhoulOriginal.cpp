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
		
	switch (awake_behaviour)
	{
	case AwakeBehaviour::FOLLOW_CURVE:

		Curve& curve = awake_curve->GetComponent<ComponentCurve>()->curve;

		current_curve_point += curve_speed * Time::GetScaleTime() * Time::GetDT();
		if (current_curve_point >= 1.f)
		{
			LOG("Enemy has completed a curve cycle!!"); 
			current_curve_point = 0.f;
			current_curve_point += curve_speed * Time::GetScaleTime() * Time::GetDT();
		}

		float3 current_position = game_object->transform->GetGlobalPosition(); 
		float3 next_position = curve.ValueAt(current_curve_point);
		next_position.y = current_position.y;
		float3 direction = (next_position - current_position).Normalized(); 
		
		Move(direction);
 
		LOG("Enemy curve current point: %f", current_curve_point);

		break; 
	}


}