#include "GhoulOriginal.h"
#include "EnemyManager.h"

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

void GhoulOriginal::Action()
{
    // Check if inside range or just entered
    if (distance < stats["AttackRange"].GetValue())
    {
        animator->PlayState("Slash");
        state = GhoulState::ATTACK;
    }
    else if (distance < stats["JumpRange"].GetValue() && distance > stats["AttackRange"].GetValue())
    {
        animator->PlayState("Jump");
        state = GhoulState::JUMP;
    }
}

void GhoulOriginal::CheckDistance()
{
    if (distance < stats["JumpRange"].GetValue())
    {
        animator->SetFloat("speed", 0.0F);
        character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
        Action();
    }

    if (distance > stats["VisionRange"].GetValue())
    {
        state = GhoulState::IDLE;
        character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
        animator->SetFloat("speed", 0.0F);
        is_combat = false;
    }
}

void GhoulOriginal::OnAnimationEnd(const char* name)
{
    if (strcmp(name, "Slash") == 0) {
        if (distance < stats["VisionRange"].GetValue() && distance > stats["JumpRange"].GetValue())
        {
            state = GhoulState::MOVE;
        }
        else
        {
            state = GhoulState::IDLE;
        }
    }
    else if (strcmp(name, "Jump") == 0)
    {
        if (distance < stats["VisionRange"].GetValue())
            state = GhoulState::MOVE;
        else
            state = GhoulState::IDLE;
    }
    else if (strcmp(name, "Hit") == 0)
        state = GhoulState::IDLE;
}