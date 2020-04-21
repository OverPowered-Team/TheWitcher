#include "Ghoul.h"

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
        // Set Attack Range and Jump Range
        stats["AttackRange"] = Stat("AttackRange", stat->GetNumber("AttackRange"));
        stats["AttackRange"].SetMaxValue(stat->GetNumber("JumpAttackRange"));
        stats["VisionRange"] = Stat("VisionRange", stat->GetNumber("VisionRange"));
    }

    JSONfilepack::FreeJSON(stat);
}

void Ghoul::StartEnemy()
{
	type = EnemyType::GHOUL;
	Enemy::StartEnemy();
}

void Ghoul::UpdateEnemy()
{
    Enemy::UpdateEnemy();
	switch (state)
	{
    case Enemy::EnemyState::IDLE:
        if (distance < stats["VisionRange"].GetValue())
            state = Enemy::EnemyState::MOVE;
        break;
    case Enemy::EnemyState::MOVE:
        Move(direction);
        break;
    case Enemy::EnemyState::ATTACK:
        Attack();
        break;
	}
}

void Ghoul::CleanUpEnemy()
{
}

void Ghoul::Attack()
{
    // Check if inside range or just entered
    if (distance < stats["AttackRange"].GetValue())
    {
        animator->PlayState("Slash");
    }
    else if (distance < stats["AttackRange"].GetMaxValue() && distance > stats["AttackRange"].GetValue())
    {
        animator->PlayState("Jump");
    }
}

void Ghoul::JumpImpulse()
{
    float force = (distance * maxForce) / stats["AttackRange"].GetValue();
    //character_ctrl->SetWalkDirection(direction * force);
}

void Ghoul::OnAnimationEnd(const char* name)
{
    if (strcmp(name, "Attack") == 0 || strcmp(name, "Jump") == 0) {
        if (distance < stats["VisionRange"].GetValue() && distance > stats["AttackRange"].GetMaxValue())
        {
            state = Enemy::EnemyState::MOVE;
        }
        else if(distance < stats["AttackRange"].GetValue())
        {
            animator->PlayState("Attack");
        }
        else if (distance < stats["AttackRange"].GetMaxValue() && distance > stats["AttackRange"].GetValue())
        {
            animator->PlayState("Jump");
        }
    }

    if (strcmp(name, "Hit") == 0) {
        state = Enemy::EnemyState::IDLE;
    }
}
