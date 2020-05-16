#include "GameManager.h"
#include "ParticlePool.h"
#include "Ghoul.h"
#include "MusicController.h"
#include "EnemyManager.h"
#include "PlayerAttacks.h"
#include "PlayerController.h"
#include "MusicController.h"
#include "GhoulOriginal.h"
#include "EnemyGroupLogic.h"
#include "Stat.h"

Ghoul::Ghoul() : Enemy()
{
}

Ghoul::~Ghoul()
{
}

void Ghoul::StartEnemy()
{
    type = EnemyType::GHOUL;
    state = GhoulState::AWAKE;
    m_controller = Camera::GetCurrentCamera()->game_object_attached->GetComponent<MusicController>();
    Enemy::StartEnemy();
}

void Ghoul::SetStats(const char* json)
{
    std::string json_path = ENEMY_JSON + std::string(json) + std::string(".json");
    LOG("READING ENEMY STAT GAME JSON WITH NAME %s", json_path.data());

    JSONfilepack* stat = JSONfilepack::GetJSON(json_path.c_str());

    JSONArraypack* stat_weapon = stat->GetArray("Ghoul");
    int i = 0;
    if (stat_weapon)
    {
        stat_weapon->GetFirstNode();
        for (int i = 0; i < stat_weapon->GetArraySize(); ++i)
            if (stat_weapon->GetNumber("Type") != (int)ghoul_type)
                stat_weapon->GetAnotherNode();
            else
                break;

        stats["Health"] = Stat("Health", stat_weapon->GetNumber("Health"));
		stats["Health"].SetMaxValue(stat_weapon->GetNumber("MaxHealth"));
		stats["Health"].SetMinValue(stat_weapon->GetNumber("MinHealth"));
        stats["Agility"] = Stat("Agility", stat_weapon->GetNumber("Agility"));
		stats["Agility"].SetMaxValue(stat_weapon->GetNumber("MaxAgility"));
		stats["Agility"].SetMinValue(stat_weapon->GetNumber("MinAgility"));
        stats["Damage"] = Stat("Damage", stat_weapon->GetNumber("Damage"));
		stats["Damage"].SetMaxValue(stat_weapon->GetNumber("MaxDamage"));
		stats["Damage"].SetMinValue(stat_weapon->GetNumber("MinDamage"));
        stats["AttackSpeed"] = Stat("AttackSpeed", stat_weapon->GetNumber("AttackSpeed"));
		stats["AttackSpeed"].SetMaxValue(stat_weapon->GetNumber("MaxAttackSpeed"));
		stats["AttackSpeed"].SetMinValue(stat_weapon->GetNumber("MinAttackSpeed"));
        stats["AttackRange"] = Stat("AttackRange", stat_weapon->GetNumber("AttackRange"));
        stats["JumpRange"] = Stat("JumpRange", stat_weapon->GetNumber("JumpAttackRange"));
        stats["VisionRange"] = Stat("VisionRange", stat_weapon->GetNumber("VisionRange"));
        stats["JumpForce"] = Stat("JumpForce", stat_weapon->GetNumber("JumpForce"));
        stats["HitSpeed"] = Stat("HitSpeed", stat_weapon->GetNumber("HitSpeed"));
        stats["HitSpeed"].SetMaxValue(stat_weapon->GetNumber("MaxHitSpeed"));
    }

    JSONfilepack::FreeJSON(stat);
}

void Ghoul::CleanUpEnemy()
{
	Enemy::CleanUpEnemy(); 

	auto script = GetComponent<GhoulOriginal>();  // TODO: do this in other enemies
	if (script)
	{
		auto curve = script->awake_curve; 
		if (curve)
			curve->ToDelete(); 
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
        animator->SetCurrentStateSpeed(stats["AttackSpeed"].GetValue());
        state = GhoulState::ATTACK;
    }
}

void Ghoul::Stun(float time)
{
    if (state != GhoulState::STUNNED && state != GhoulState::DEAD && state != GhoulState::DYING)
    {
        state = GhoulState::STUNNED;
        animator->PlayState("Dizzy");
        current_stun_time = Time::GetGameTime();
        audio_emitter->StartSound("Play_Dizzy_Enemy");
        stun_time = time;
    }
}

bool Ghoul::IsDead()
{
    return (state == GhoulState::DEAD ? true : false);
}

void Ghoul::SetState(const char* state_str)
{
	if (state_str == "Awake")
		state = GhoulState::AWAKE;
    else if (state_str == "Idle")
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

void Ghoul::Action()
{
    // Check if inside range or just entered
    if (distance < stats["AttackRange"].GetValue())
    {
        animator->PlayState("Slash");
        animator->SetCurrentStateSpeed(stats["AttackSpeed"].GetValue());
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
	if (state == GhoulState::AWAKE)
		return; 

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
        if (m_controller && is_combat) {
            LOG("Adios");
            is_combat = false;
            m_controller->EnemyLostSight((Enemy*)this);
        }
        LOG("Adios");
    }
    if (distance < stats["VisionRange"].GetValue()) {
        if (m_controller && !is_combat)
        {
            LOG("HOLA");
            is_combat = true;
            m_controller->EnemyInSight((Enemy*)this);
        }
        LOG("HOLA");
    }
}

float Ghoul::GetDamaged(float dmg, PlayerController* player, float3 knock_back)
{
    state = GhoulState::HIT;
    float damage = Enemy::GetDamaged(dmg, player, knock_back);

    if (can_get_interrupted || stats["Health"].GetValue() == 0.0F) {
        animator->PlayState("Hit");
        audio_emitter->StartSound("GhoulHit");
        stats["HitSpeed"].IncreaseStat(increase_hit_animation);
        animator->SetCurrentStateSpeed(stats["HitSpeed"].GetValue());
    }

    if (stats["HitSpeed"].GetValue() == stats["HitSpeed"].GetMaxValue())
    {
        stats["HitSpeed"].SetCurrentStat(stats["HitSpeed"].GetBaseValue());
        animator->SetCurrentStateSpeed(stats["HitSpeed"].GetValue());
        can_get_interrupted = false;
    }


    SpawnParticle("hit_particle", particle_spawn_positions[1]->transform->GetLocalPosition());

    character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);

    if (stats["Health"].GetValue() == 0.0F) {

        animator->SetBool("dead", true);
        OnDeathHit();
        state = GhoulState::DYING;
        audio_emitter->StartSound("GhoulDeath");
    }

    return damage;
}

void Ghoul::OnTriggerEnter(ComponentCollider* collider)
{
    if (strcmp(collider->game_object_attached->GetTag(), "PlayerAttack") == 0 && state != GhoulState::DEAD) {
        PlayerController* player = collider->game_object_attached->GetComponentInParent<PlayerController>();
        if (player && player->attacks->GetCurrentAttack()->CanHit(this))
        {
            float dmg_received = player->attacks->GetCurrentDMG();
            float3 knock = (this->transform->GetGlobalPosition() - player->game_object->transform->GetGlobalPosition()).Normalized();
            knock = knock * player->attacks->GetCurrentAttack()->info.stats["KnockBack"].GetValue();

            player->OnHit(this, GetDamaged(dmg_received, player, knock));
            last_player_hit = player;

            HitFreeze(player->attacks->GetCurrentAttack()->info.freeze_time);
        }
    }
}

void Ghoul::OnAnimationEnd(const char* name)
{
    if (strcmp(name, "Slash") == 0) {
        can_get_interrupted = true;
        stats["HitSpeed"].SetCurrentStat(stats["HitSpeed"].GetBaseValue());
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
    {
        ReleaseParticle("hit_particle");

        state = GhoulState::IDLE;
    }


}


void Ghoul::OnDrawGizmosSelected()
{
	if(wander_radius > 0.0f)
		Gizmos::DrawWireSphere(start_pos, wander_radius, Color::Blue());
	
}


void Ghoul::DoAwake()
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
				wander_rest = true;
				character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
				animator->SetFloat("speed", 0.0F);
			}

		}

		break;
	}

	}


}

void Ghoul::Dying() // TODO: in other enemies
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


	// Enemy leader logic -> after setting it to dead 
	GameObject* group = game_object->parent->parent;
	if (group != nullptr)
	{
		EnemyGroupLogic* logic = group->GetComponent<EnemyGroupLogic>();
		if (logic)
			logic->OnEnemyDying(game_object);

	}
}