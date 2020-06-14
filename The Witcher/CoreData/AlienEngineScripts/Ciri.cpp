#include "GameManager.h"
#include "PlayerManager.h"
#include "EnemyManager.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"
#include "CiriFightController.h"
#include "Ciri.h"
#include "Scores_Data.h"
#include "UI_DamageCount.h"

void Ciri::StartEnemy()
{
	actions.emplace("Dash", new BossAction(ActionType::DASH, 0.0f));
	actions.emplace("Combo", new BossAction(ActionType::COMBO, 0.0f));
	actions.emplace("MiniScream", new BossAction(ActionType::MINISCREAM, 0.0f));

	can_get_interrupted = true;
	action_cooldown = 0.3f;

	type = EnemyType::CIRI_CLONE;

	Boss::StartEnemy();

	fight_controller = GameObject::FindWithName("Ciri")->GetComponent<CiriFightController>();

	state = Boss::BossState::NONE;
	animator->PlayState("Spawn");
	appearing = true;

	emitter = GetComponent<ComponentAudioEmitter>();
}

void Ciri::UpdateEnemy()
{
	Boss::UpdateEnemy();
	
	if (dissolve_mat.shaderInputs.dissolveFresnelShaderProperties.burn < 1 && appearing) {
		dissolve_mat.shaderInputs.dissolveFresnelShaderProperties.burn += 0.2 * Time::GetDT();
	}
	else {
		appearing = false;
	}

	if (dissolve_mat.shaderInputs.dissolveFresnelShaderProperties.burn > 0 && disappearing) {
		dissolve_mat.shaderInputs.dissolveFresnelShaderProperties.burn -= 0.2 * Time::GetDT();
	}
	else {
		disappearing = false;
	}
}

void Ciri::CleanUpEnemy()
{
	Boss::CleanUpEnemy();
}

float Ciri::GetDamaged(float dmg, PlayerController* player, float3 knock_back)
{
	if (GameObject::FindWithName("HUD_Game")->GetChild("UI_InGame")->GetChild("InGame")->GetComponent<UI_DamageCount>())
	{
		GameObject::FindWithName("HUD_Game")->GetChild("UI_InGame")->GetChild("InGame")->GetComponent<UI_DamageCount>()->AddDamageCount(dmg, player);
	}

	float aux_health = stats["Health"].GetValue();
	stats["Health"].DecreaseStat(dmg);

	last_player_hit = player;
	velocity = knock_back; //This will replace old knockback if there was any...

	if (can_get_interrupted)
	{
		animator->PlayState("Hit");
		SetState("Hit");
	}

	float2 temp_e = float2(particle_spawn_positions[1]->transform->GetGlobalPosition().x, particle_spawn_positions[1]->transform->GetGlobalPosition().z);
	float2 temp_pl = float2(last_player_hit->transform->GetGlobalPosition().x, last_player_hit->transform->GetGlobalPosition().z);
	float angle = acos(math::Dot(temp_e, temp_pl) / (temp_e.LengthSq() * temp_pl.LengthSq())); //sino provar Length	
	float anglez = math::RadToDeg(angle);
	float3 rotated_particle = float3(
		temp_e.x * cos(anglez) - temp_pl.y * sin(anglez),
		0.0f,
		temp_e.x * sin(anglez) + temp_pl.y * cos(anglez));
	math::Quat player_quat = last_player_hit->transform->GetGlobalRotation();
	float3 particle_rotation = last_player_hit->attacks->GetCurrentAttack()->info.hit_particle_dir;

	SpawnParticle(last_player_hit->attacks->GetCurrentAttack()->info.hit_particle_name, particle_spawn_positions[1]->transform->GetGlobalPosition(), false, particle_rotation, nullptr, player_quat);
	character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);

	if (stats["Health"].GetValue() <= 0.0F) {
		animator->SetBool("dead", true);
		is_dead = true;
		OnDeathHit();

		SetState("Dying");
		PlaySFX("Death");
		animator->PlayState("Death");

		fight_controller->OnCloneDead(game_object);
	}

	HitFreeze(player->attacks->GetCurrentAttack()->info.freeze_time);
	for (auto iter = meshes.begin(); iter != meshes.end(); iter++)
	{
		if ((*iter))
		{
			//ComponentMaterial* material = (*iter)->GetComponent<ComponentMaterial>();
			if (defaultMaterial == nullptr)
			{
				defaultMaterial = (ResourceMaterial*)(*iter)->GetMaterial();
			}

			if ((*iter))
			{
				(*iter)->material = &hitMaterial;
			}
		}
		inHit = true;
	}
	return aux_health - stats["Health"].GetValue();
}

void Ciri::SetActionProbabilities()
{
	Boss::SetActionProbabilities();


	if (player_distance[0] <= mini_scream_range && player_distance[1] <= mini_scream_range && fight_controller->can_mini_scream) {
		actions.find("MiniScream")->second->probability = 100.0f;
		action_cooldown = 0.5f;
		return;
	}else if (player_distance[0] <= combo_range && player_controllers[0]->state->type != StateType::DEAD || player_distance[1] <= combo_range && player_controllers[1]->state->type != StateType::DEAD) {
		actions.find("Combo")->second->probability = 100.0f;
		action_cooldown = 0.5f;
		return;
	}
	else{
		actions.find("Dash")->second->probability = 100.0f;
		action_cooldown = 0.3f;
	}
}

void Ciri::SetActionVariables()
{
	Boss::SetActionVariables();
}

bool Ciri::IsOnAction()
{
	return current_action != nullptr;
}

void Ciri::LaunchAction()
{
	Boss::LaunchAction();

	switch (current_action->type)
	{
	case Ciri::ActionType::NONE:
		break;
	case Ciri::ActionType::DASH:
		LaunchDashAction();
		break;
	case Ciri::ActionType::COMBO:
		LaunchComboAction();
		break;
	case Ciri::ActionType::MINISCREAM:
		LaunchMiniScreamAction();
		break;
	default:
		break;
	}

	current_action->state = ActionState::UPDATING;
}

float Ciri::GetDamaged(float dmg, float3 knock_back)
{
	float aux_health = stats["Health"].GetValue();
	stats["Health"].DecreaseStat(dmg);

	velocity = knock_back; //This will replace old knockback if there was any...

	if (can_get_interrupted)
	{
		SetState("Hit");
		animator->PlayState("Hit");
		PlaySFX("Hit");
	}

	if (stats["Health"].GetValue() <= 0.0F) {
		animator->SetBool("dead", true);
		is_dead = true;
		OnDeathHit();

		SetState("Dying");
		PlaySFX("Death");

		animator->PlayState("Death");

		fight_controller->OnCloneDead(game_object);
	}

	for (auto iter = meshes.begin(); iter != meshes.end(); iter++)
	{
		if ((*iter))
		{
			//ComponentMaterial* material = (*iter)->GetComponent<ComponentMaterial>();
			if (defaultMaterial == nullptr)
			{
				defaultMaterial = (ResourceMaterial*)(*iter)->GetMaterial();
			}

			if ((*iter))
			{
				(*iter)->material = &hitMaterial;
			}
		}
		inHit = true;
	}
	return aux_health - stats["Health"].GetValue();
}

void Ciri::LaunchDashAction()
{
	//dash_direction = (player_controllers[player_distance[0] > player_distance[1] ? 1 : 0]->game_object->transform->GetGlobalPosition() - this->transform->GetGlobalPosition()).Normalized();
	//target = player_distance[0] > player_distance[1] ? 1 : 0;
	if (player_controllers[0]->state->type == StateType::DEAD)
		target = 1;
	else if (player_controllers[1]->state->type == StateType::DEAD)
		target = 0;
	else {
		target = Random::GetRandomIntBetweenTwo(0, 1);
	}
	dash_direction = (player_controllers[target]->game_object->transform->GetGlobalPosition() - this->transform->GetGlobalPosition()).Normalized();
	dash_direction.y = 0;
	animator->PlayState("Dash");
	animator->SetStateSpeed("Dash", 1.0f);

	emitter->StartSound("Play_CiriClone_Dash");

	OrientToPlayerWithoutSlerp(target);
}

void Ciri::LaunchComboAction()
{
	animator->PlayState("Combo");
	CheckForGapCloser();

	if (player_controllers[0]->state->type == StateType::DEAD)
		target = 1;
	else if (player_controllers[1]->state->type == StateType::DEAD)
		target = 0;
	else {
		target = player_distance[0] > player_distance[1] ? 1 : 0;
	}
}

void Ciri::LaunchMiniScreamAction()
{
	fight_controller->can_mini_scream = false;
	animator->PlayState("Scream");
	fight_controller->scream_cd_timer = 0;
	SpawnParticle("Ciri_MiniScream", { 0, 0.6, 0 }, true);
	emitter->StartSound("Play_CiriClone_Scream");
}

void Ciri::SetAsCanGetInterrupted()
{
	can_get_interrupted = true;
}

void Ciri::SetAsCantGetInterrupted()
{
	can_get_interrupted = false;
}

void Ciri::MiniScream()
{
	if (player_distance[0] <= mini_scream_range && player_controllers[0]->state->type != StateType::DEAD) {
		float3 knockbak_direction = (player_controllers[0]->transform->GetGlobalPosition() - this->transform->GetGlobalPosition()).Normalized();
		player_controllers[0]->ReceiveDamage(mini_scream_damage, knockbak_direction * mini_scream_force);
	}

	if (player_distance[1] <= mini_scream_range && player_controllers[1]->state->type != StateType::DEAD) {
		float3 knockbak_direction = (player_controllers[1]->transform->GetGlobalPosition() - this->transform->GetGlobalPosition()).Normalized();
		player_controllers[1]->ReceiveDamage(mini_scream_damage, knockbak_direction * mini_scream_force);

	}
}

Boss::ActionState Ciri::UpdateAction()
{
	switch (current_action->type)
	{
	case Ciri::ActionType::NONE:
		break;
	case Ciri::ActionType::DASH:
		current_action->state = UpdateDashAction();
		break;
	case Ciri::ActionType::COMBO:
		current_action->state = UpdateComboAction();
		break;
	case Ciri::ActionType::MINISCREAM:
		current_action->state = UpdateMiniScreamAction();
		break;
	default:
		break;
	}

	return current_action->state;
}

Boss::ActionState Ciri::UpdateDashAction()
{
	//character_ctrl->Move(dash_direction * stats["Agility"].GetValue());
	if (player_distance[0] > combo_range || player_distance[1] > combo_range) {
		character_ctrl->Move((dash_direction * dash_speed) * Time::GetDT());

		LOG("UPDATING DASH ACTION");

		if (dash_timer <= dash_time) {
			dash_timer += Time::GetDT();
		}
		else {
			character_ctrl->Move(float3::zero());
			dash_timer = 0;
			current_action->state = Boss::ActionState::ENDED;
			animator->SetStateSpeed("Dash", 0.0f);
		}
	}
	else {
		dash_timer = 0;
		current_action->state = Boss::ActionState::ENDED;
		character_ctrl->Move(float3::zero());
	}

	return current_action->state;
}

Boss::ActionState Ciri::UpdateComboAction()
{
	LOG("UPDATING COMBO ACTION");

	OrientToPlayer(target);

	if (gap_closer) {
		float3 gap_closer_direction = (player_controllers[target]->game_object->transform->GetGlobalPosition() - this->transform->GetGlobalPosition()).Normalized();
		character_ctrl->Move((gap_closer_direction * gap_closer_speed) * Time::GetDT());
		if (gap_closer_speed > 0)
			gap_closer_speed -= gap_closer_decay;
		else
			gap_closer_speed = 0;
		if (player_distance[target] < combo_range)
			gap_closer = false;
	}

	return current_action->state;
}

Boss::ActionState Ciri::UpdateMiniScreamAction()
{
	LOG("UPDATING MiniScream ACTION");

	return current_action->state;
}

void Ciri::CheckForGapCloser()
{
	if (player_distance[target] > combo_range) {
		gap_closer = true;
		gap_closer_speed = 4.0f;
	}
}

void Ciri::EndAction(GameObject* go_ended)
{
}

void Ciri::EndDashAction(GameObject* go_ended)
{
}

void Ciri::EndComboAction(GameObject* go_ended)
{
	current_action->state = Boss::ActionState::ENDED;
}

void Ciri::EndMiniScreamAction(GameObject* go_ended)
{
}

void Ciri::OnAnimationEnd(const char* name)
{
	if (strcmp(name, "Combo3") == 0) {
		stats["HitSpeed"].SetCurrentStat(stats["HitSpeed"].GetBaseValue());
		EndComboAction(nullptr);
	}
	if (strcmp(name, "Combo") == 0) {
		stats["HitSpeed"].SetCurrentStat(stats["HitSpeed"].GetBaseValue());
		CheckForGapCloser();
	}
	if (strcmp(name, "Combo2") == 0) {
		stats["HitSpeed"].SetCurrentStat(stats["HitSpeed"].GetBaseValue());
		CheckForGapCloser();
	}
	if (strcmp(name, "Scream") == 0) {
		current_action->state = Boss::ActionState::ENDED;
		ReleaseParticle("Ciri_MiniScream");
	}
	if (strcmp(name, "Spawn") == 0) {
		state = Boss::BossState::IDLE;
	}

	if (strcmp(name, "Hit") == 0) {
		if (current_action) {
			current_action->state = Boss::ActionState::ENDED;
		}
		state = BossState::IDLE;
	}
}

void Ciri::OnCloneDeath()
{

}

void Ciri::OnDrawGizmosSelected()
{
	Gizmos::DrawWireSphere(transform->GetGlobalPosition(), mini_scream_range, Color::Cyan()); //snap_range
	Gizmos::DrawWireSphere(transform->GetGlobalPosition(), scream_range, Color::Cyan()); //snap_range
}

void Ciri::SetStats(const char* json)
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
		stats["VisionRange"] = Stat("VisionRange", stat->GetNumber("VisionRange"));
		stats["KnockBack"] = Stat("KnockBack", stat->GetNumber("KnockBack"));
		stats["HitSpeed"] = Stat("HitSpeed", stat->GetNumber("HitSpeed"));
		stats["HitSpeed"].SetMaxValue(stat->GetNumber("MaxHitSpeed"));
	}

	JSONfilepack::FreeJSON(stat);
}


bool Ciri::IsHit()
{
	if (state == BossState::HIT)
		return true;
	else
		return false;
}

void Ciri::SetState(const char* state)
{
	if (state == "Hit")
		this->state = BossState::HIT;
	else if (state == "Dying")
		this->state = BossState::DYING;
	else if (state == "Dead")
		this->state = BossState::DEAD;
}
