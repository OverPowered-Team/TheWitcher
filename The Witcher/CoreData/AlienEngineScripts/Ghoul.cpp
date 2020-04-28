#include "Ghoul.h"
#include "MusicController.h"
#include "EnemyManager.h"

Ghoul::Ghoul() : Enemy()
{
}

Ghoul::~Ghoul()
{
}

void Ghoul::StartEnemy()
{
    type = EnemyType::GHOUL;
    state = GhoulState::IDLE;
    Enemy::StartEnemy();
}

void Ghoul::SetStats(const char* json)
{
    std::string json_path = ENEMY_JSON + std::string(json) + std::string(".json");
    LOG("READING ENEMY STAT GAME JSON WITH NAME %s", json_path.data());

    JSONfilepack* stat = JSONfilepack::GetJSON(json_path.c_str());
    if (stat)
    {
        stats["Health"] = Stat("Health", stat->GetNumber("Health"));
        stats["Agility"] = Stat("Agility", stat->GetNumber("Agility"));
        stats["Damage"] = Stat("Damage", stat->GetNumber("Damage"));
        stats["AttackSpeed"] = Stat("AttackSpeed", stat->GetNumber("AttackSpeed"));
        stats["AttackRange"] = Stat("AttackRange", stat->GetNumber("AttackRange"));
        stats["JumpRange"] = Stat("JumpRange", stat->GetNumber("JumpAttackRange"));
        stats["VisionRange"] = Stat("VisionRange", stat->GetNumber("VisionRange"));
        stats["JumpForce"] = Stat("JumpForce", stat->GetNumber("JumpForce"));
    }

    JSONfilepack::FreeJSON(stat);
}

void Ghoul::UpdateEnemy()
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
        audio_emitter->StartSound("GhoulDeath");
        state = GhoulState::DEAD;
        break;
    }
    }
}

void Ghoul::CleanUpEnemy()
{
}

void Ghoul::Action() 
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

void Ghoul::CheckDistance()
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

void Ghoul::JumpImpulse()
{
    if (distance > stats["AttackRange"].GetValue())
    {
        float3 jump_direction = direction * stats["Agility"].GetValue() * stats["JumpForce"].GetValue();
        character_ctrl->Move(jump_direction * Time::GetDT() * Time::GetScaleTime());
    }
    else
    {
        animator->PlayState("Slash");
        state = GhoulState::ATTACK;
    }
}

void Ghoul::Stun(float time)
{
    if (state != GhoulState::STUNNED)
    {
        state = GhoulState::STUNNED;
        //animator->PlayState("Dizzy");
        current_stun_time = Time::GetGameTime();
        stun_time = time;
    }
}

bool Ghoul::IsDead()
{
    return (state == GhoulState::DEAD ? true : false);
}

void Ghoul::OnAnimationEnd(const char* name)
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
