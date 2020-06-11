#include "GameManager.h"
#include "PlayerManager.h"
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
    state = GhoulState::IDLE;
    decapitation_particle = "decapitation_particle_ghoul";
    m_controller = Camera::GetCurrentCamera()->game_object_attached->GetComponent<MusicController>();
    range_collider = game_object->GetChild("EnemyRangeAttack")->GetComponent<ComponentBoxCollider>();
    range_collider->SetEnable(false);
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
        stats["Agility"] = Stat("Agility", stat_weapon->GetNumber("Agility"));
        stats["Damage"] = Stat("Damage", stat_weapon->GetNumber("Damage"));
        stats["AttackSpeed"] = Stat("AttackSpeed", stat_weapon->GetNumber("AttackSpeed"));
        stats["AttackRange"] = Stat("AttackRange", stat_weapon->GetNumber("AttackRange"));
        stats["JumpRange"] = Stat("JumpRange", stat_weapon->GetNumber("JumpAttackRange"));
        stats["VisionRange"] = Stat("VisionRange", stat_weapon->GetNumber("VisionRange"));
        stats["MoveRange"] = Stat("MoveRange", stat_weapon->GetNumber("MoveRange"));
        stats["JumpForce"] = Stat("JumpForce", stat_weapon->GetNumber("JumpForce"));
        stats["HitSpeed"] = Stat("HitSpeed", stat_weapon->GetNumber("HitSpeed"));
        stats["Acceleration"] = Stat("Acceleration", stat_weapon->GetNumber("Acceleration"));
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
    if (can_jump)
    {
        character_ctrl->Move(jump_direction * Time::GetDT() * Time::GetScaleTime());
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
bool Ghoul::IsHit()
{
    return (state == GhoulState::HIT ? true: false);
}
bool Ghoul::IsDying()
{
    return (state == GhoulState::DYING ? true : false);
}


void Ghoul::SetState(const char* state_str)
{
	if (state_str == "Awake")
		state = GhoulState::AWAKE;
    if (state_str == "Idle")
    {
        state = GhoulState::IDLE;
        character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
        velocity = float3::zero();
        animator->SetFloat("speed", 0.0F);
    }
    else if (state_str == "Move")
    {
        state = GhoulState::MOVE;
    }
    else if (state_str == "Attack")
    {
        state = GhoulState::ATTACK;
        animator->SetFloat("speed", 0.0F);
        character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
        velocity = float3::zero();
    }
    else if (state_str == "Guard")
    {
        state = GhoulState::GUARD;
        character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
        velocity = float3::zero();
        animator->SetFloat("speed", 0.0F);
    }
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

bool Ghoul::IsRangeEnemy()
{
    if (ghoul_type == GhoulType::MINI)
        return false;
    else
        return true;
}

void Ghoul::Action()
{
    // Check if inside range or just entered
    if (distance < stats["AttackRange"].GetValue())
    {
        animator->PlayState("Slash");
        RotatePlayer();
        animator->SetCurrentStateSpeed(stats["AttackSpeed"].GetValue());
        state = GhoulState::ATTACK;
    }
    else if (distance < stats["MoveRange"].GetValue() && distance > stats["AttackRange"].GetValue())
    {
        state = GhoulState::MOVE;
    }
    else if (distance < stats["JumpRange"].GetValue() && distance > stats["MoveRange"].GetValue())
    {
        RotatePlayer();
        animator->PlayState("Jump");
        jump_speed = (distance + 1) / 1.13f * animator->GetCurrentStateSpeed();
        jump_direction = direction.Normalized() * jump_speed;
        state = GhoulState::JUMP;
    }

}

void Ghoul::CheckDistance()
{
	if (state == GhoulState::AWAKE)
		return; 

    if (distance < stats["VisionRange"].GetValue() || is_obstacle)
    {
        Action();
    }
    else if (distance > stats["VisionRange"].GetValue() && !is_obstacle)
    {
        SetState("Idle");
        if (m_controller && is_combat) {
            is_combat = false;
            m_controller->EnemyLostSight((Enemy*)this);
        }
    }
    if (distance < stats["VisionRange"].GetValue()) {
        if (m_controller && !is_combat)
        {
            is_combat = true;
            m_controller->EnemyInSight((Enemy*)this);
        }
    }
}

void Ghoul::OnAnimationEnd(const char* name)
{
    if (strcmp(name, "Slash") == 0) {
        can_get_interrupted = true;
        //stats["HitSpeed"].SetCurrentStat(stats["HitSpeed"].GetBaseValue());
        //animator->SetCurrentStateSpeed(stats["HitSpeed"].GetValue());
        ReleaseParticle("EnemyAttackParticle");
        if (distance < stats["VisionRange"].GetValue() && distance > stats["JumpRange"].GetValue())
            SetState("Move");
        else
            SetState("Idle");
        animator->SetBool("attack", false);
    }
    else if (strcmp(name, "Jump") == 0)
    {
        if (distance < stats["AttackRange"].GetValue())
            SetState("Attack");
        else if (distance < stats["VisionRange"].GetValue() && distance > stats["MoveRange"].GetValue())
            SetState("Move");
        else
            SetState("Idle");


        can_jump = false;

        if (ghoul_type != GhoulType::MINI)
        {
            ReleaseParticle("AreaAttackSlash");
            ReleaseParticle("AreaAttackSphere");
            ReleaseParticle("AreaAttackRock");
        }
        else
        {
            ReleaseParticle("AreaAttackSlashMini");
            ReleaseParticle("AreaAttackSphereMini");
            ReleaseParticle("AreaAttackRockMini");
        }
    }
    else if (strcmp(name, "Hit") == 0)
    {
        ReleaseParticle("hit_particle");

        if (stats["Health"].GetValue() <= 0.0F)
        {
            if (!was_dizzy)
                was_dizzy = true;
            else
            {
                state = GhoulState::DYING;
                GameManager::instance->player_manager->IncreaseUltimateCharge(10);
            }
        }
        /*else if (is_attacking)
            ChangeAttackEnemy();*/
        else if (!is_dead)
            SetState("Idle");

        animator->SetBool("attack", false);
    }
    else if ((strcmp(name, "Dizzy") == 0) && stats["Health"].GetValue() <= 0)
    {
        state = GhoulState::DYING;
        GameManager::instance->player_manager->IncreaseUltimateCharge(10);
    }
}


void Ghoul::OnDrawGizmosSelected()
{
	if(awake_behaviour == AwakeBehaviour::WANDER)
		Gizmos::DrawWireSphere(start_pos, wander_radius, Color::Blue());
	
}

void Ghoul::ActivateRangeCollider()
{
    range_collider->SetEnable(true);
    if (ghoul_type != GhoulType::MINI) 
    {
        SpawnParticle("AreaAttackSlash", particle_spawn_positions[2]->transform->GetGlobalPosition());
        SpawnParticle("AreaAttackSphere", particle_spawn_positions[2]->transform->GetGlobalPosition());
        SpawnParticle("AreaAttackRock", particle_spawn_positions[2]->transform->GetGlobalPosition());
    }
    else
    {
        SpawnParticle("AreaAttackSlashMini", particle_spawn_positions[2]->transform->GetGlobalPosition());
        SpawnParticle("AreaAttackSphereMini", particle_spawn_positions[2]->transform->GetGlobalPosition());
        SpawnParticle("AreaAttackRockMini", particle_spawn_positions[2]->transform->GetGlobalPosition());
    }
}

void Ghoul::DeactivateRangeCollider()
{
    range_collider->SetEnable(false);
}

void Ghoul::PlaySFX(const char* sfx_name)
{
    if (sfx_name == "Hit")
        audio_emitter->StartSound("GhoulHit");
    else if (sfx_name == "Death")
        audio_emitter->StartSound("GhoulDeath");
    else
        LOG("Sound effect with name %s not found!", sfx_name);
}

bool Ghoul::IsState(const char* state_str)
{
    if (state_str == "Idle")
        return (state == GhoulState::IDLE ? true : false);
    else if (state_str == "Move")
        return (state == GhoulState::MOVE ? true : false);
    else if (state_str == "Attack")
        return (state == GhoulState::ATTACK ? true : false);
    else if (state_str == "Hit")
        return (state == GhoulState::HIT ? true : false);
    else if (state_str == "Dying")
        return (state == GhoulState::DYING ? true : false);
    else if (state_str == "Stunned")
        return (state == GhoulState::STUNNED ? true : false);
    else
        LOG("Incorrect state name: %s", state_str);
}

void Ghoul::CanJump()
{
    can_jump = true;
}

void Ghoul::CanNotJump()
{
    can_jump = false;
}

void Ghoul::DoAwake() // Do this in other enemies
{
	if (is_combat)
	{
		Move(direction);
		return; 
	}

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

		if(patrol)
			LOG("Current patrol enemy curve point: %f", current_curve_point); 

		// Go forwards
		if(!patrol)
			current_curve_point += curve_speed * Time::GetScaleTime() * Time::GetDT();
		else
		{
			// Go forwards or backwards
			if(curve_patrol_go)
				current_curve_point += curve_speed * Time::GetScaleTime() * Time::GetDT();
			else
				current_curve_point -= curve_speed * Time::GetScaleTime() * Time::GetDT();
		}
		
		// Reached end
		if (((current_curve_point >= 1.f)) || ((current_curve_point < 0.f) && patrol))
		{
			// Current point set to the beginning
			
			// If patrol, now go the other way around 
			if (patrol)
			{
				// Cap the current point to either 1 or 0
				if (current_curve_point >= 1.f)
					current_curve_point = 1.f; 
				else
					current_curve_point = 0.f;

				// Go the other way around
				curve_patrol_go = !curve_patrol_go;

				// Speed has a + or - multiplier
				if (curve_patrol_go)
					current_curve_point += curve_speed * Time::GetScaleTime() * Time::GetDT();
				else
					current_curve_point -= curve_speed * Time::GetScaleTime() * Time::GetDT();
			}
			else
			{
				// Otherwise just keep going
				current_curve_point = 0.f;
				current_curve_point += curve_speed * Time::GetScaleTime() * Time::GetDT();
			}
			
	
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
	last_player_hit->OnEnemyKill((uint)type);
	state = GhoulState::DEAD;
	if (m_controller && is_combat)
	{
		is_combat = false;
		m_controller->EnemyLostSight((Enemy*)this);
	}
    if (is_obstacle)
    {
        game_object->parent->parent->GetComponent<BlockerObstacle>()->ReleaseMyself(this);
    }

	// Enemy leader logic -> after setting it to dead 
	/*GameObject* group = game_object->parent->parent;
	if (group != nullptr)
	{
		EnemyGroupLogic* logic = group->GetComponent<EnemyGroupLogic>();
		if (logic)
			logic->OnEnemyDying(game_object);

	}*/
}