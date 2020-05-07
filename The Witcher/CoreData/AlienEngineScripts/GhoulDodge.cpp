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

    switch (state)
    {
    case GhoulState::IDLE:
        if (distance < stats["VisionRange"].GetValue())
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

    case GhoulState::DODGE:
        Dodge();
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

