#include "Ghoul.h"
#include "MusicController.h"
#include "EnemyManager.h"
#include "PlayerAttacks.h"
#include "PlayerController.h"

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

void Ghoul::CleanUpEnemy()
{
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

void Ghoul::SetState(const char* state_str)
{
    if (state_str == "Idle")
        state = GhoulState::IDLE;
    else if (state_str == "Move")
        state = GhoulState::MOVE;
    else if (state_str == "Attack")
        state = GhoulState::ATTACK;
    else if (state_str == "Jump")
        state = GhoulState::JUMP;
    else if (state_str == "Hit")
        state = GhoulState::HIT;
    else if (state_str == "Dying")
        state = GhoulState::DYING;
    else if (state_str == "Dead")
        state = GhoulState::DEAD;
    else if (state_str == "Stunned")
        state = GhoulState::STUNNED;
    else
        LOG("Incorrect state name: %s", state_str);
}

float Ghoul::GetDamaged(float dmg, PlayerController* player)
{
    float damage = Enemy::GetDamaged(dmg, player);

    if (can_get_interrupted) {
        state = GhoulState::HIT;
        animator->PlayState("Hit");
    }

    audio_emitter->StartSound("GhoulHit");
    if (particles["hit_particle"])
        particles["hit_particle"]->Restart();

    character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);

    if (stats["Health"].GetValue() == 0.0F) {

        animator->SetBool("dead", true);
        OnDeathHit();
        state = GhoulState::DYING;
        audio_emitter->StartSound("GhoulDeath");
        player->OnEnemyKill();
    }

    return damage;
}

void Ghoul::OnTriggerEnter(ComponentCollider* collider)
{
    if (strcmp(collider->game_object_attached->GetTag(), "PlayerAttack") == 0 && state != GhoulState::DEAD) {
        PlayerController* player = collider->game_object_attached->GetComponentInParent<PlayerController>();
        if (player)
        {
            float dmg_received = player->attacks->GetCurrentDMG();
            player->OnHit(this, GetDamaged(dmg_received, player));

            if (state == GhoulState::DYING)
                player->OnEnemyKill();

            HitFreeze(player->attacks->GetCurrentAttack()->info.freeze_time);
        }
    }
}


