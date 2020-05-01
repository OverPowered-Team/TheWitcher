#include "GhoulDodge.h"
#include "EnemyManager.h"

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
    case GhoulState::DYING:
    {
        EnemyManager* enemy_manager = GameObject::FindWithName("GameManager")->GetComponent< EnemyManager>();
        //Ori Ori function sintaxis
        Invoke([enemy_manager, this]() -> void {enemy_manager->DeleteEnemy(this); }, 5);
        animator->PlayState("Death");
        audio_emitter->StartSound("GhoulDeath");
        state = GhoulState::DEAD;
        break;
    }
    }
}
