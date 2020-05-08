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