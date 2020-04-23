#include "Ghoul.h"
#include "MusicController.h"
#include "EnemyManager.h"

Ghoul::Ghoul() : Enemy()
{
}

Ghoul::~Ghoul()
{
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

void Ghoul::StartEnemy()
{
	type = EnemyType::GHOUL;
    m_controller = Camera::GetCurrentCamera()->game_object_attached->GetComponent<MusicController>();
	Enemy::StartEnemy();
}

void Ghoul::UpdateEnemy()
{
    Enemy::UpdateEnemy();

	switch (state)
	{
    case Enemy::EnemyState::IDLE:
        if (distance < stats["VisionRange"].GetValue())
        {
            state = Enemy::EnemyState::MOVE;
            m_controller->is_combat = true; //Note: This should be placed to every enemy type and not especifically in each enemy
            m_controller->has_changed = true;
        }
        break;
    case Enemy::EnemyState::MOVE:
        if (distance > stats["VisionRange"].GetValue())
        {
            m_controller->is_combat = false;
            m_controller->has_changed = true;
        }
        Move(direction);
        break;
    case Enemy::EnemyState::BLOCK:
            JumpImpulse();
            break;
    case Enemy::EnemyState::ATTACK:
        break;
    case Enemy::EnemyState::DYING:
    {
        EnemyManager* enemy_manager = GameObject::FindWithName("GameManager")->GetComponent< EnemyManager>();
        //Ori Ori function sintaxis
        Invoke([enemy_manager, this]() -> void {enemy_manager->DeleteEnemy(this); }, 5);
        audio_emitter->StartSound("GhoulDeath");
        state = EnemyState::DEAD;
        m_controller->is_combat = false;
        m_controller->has_changed = true;
        break;
    }
    default:
        LOG("There's no state");
        break;
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
        state = EnemyState::ATTACK;
    }
    else if (distance < stats["JumpRange"].GetValue() && distance > stats["AttackRange"].GetValue())
    {
        animator->PlayState("Jump");
        state = EnemyState::BLOCK;
    }
}

void Ghoul::JumpImpulse()
{
    if (distance > stats["AttackRange"].GetValue())
    {
        float3 jump_direction = direction * stats["Agility"].GetValue() * stats["JumpForce"].GetValue();
        character_ctrl->Move(jump_direction);
    }
    else
    {
        animator->PlayState("Slash");
        state = EnemyState::ATTACK;
    }
}

void Ghoul::OnAnimationEnd(const char* name)
{
    if (strcmp(name, "Slash") == 0) {
        if (distance < stats["VisionRange"].GetValue() && distance > stats["JumpRange"].GetValue())
        {
            state = Enemy::EnemyState::MOVE;
        }
        else
        {
            state = Enemy::EnemyState::IDLE;
        }
    }
    else if (strcmp(name, "Jump") == 0)
    {
        if (distance < stats["VisionRange"].GetValue())
        {
            state = Enemy::EnemyState::MOVE;
        }
        else
        {
            state = Enemy::EnemyState::IDLE;
        }
    }
    else if (strcmp(name, "Hit") == 0) {
        state = Enemy::EnemyState::IDLE;
    }
}
