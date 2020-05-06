#include "GameManager.h"
#include "PlayerManager.h"
#include "EnemyManager.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"
#include "CiriFightController.h"
#include "Ciri.h"


void Ciri::StartEnemy()
{
	actions.emplace("Dash", new BossAction(ActionType::DASH, 0.0f));
	actions.emplace("Combo", new BossAction(ActionType::COMBO, 0.0f));
	actions.emplace("MiniScream", new BossAction(ActionType::MINISCREAM, 0.0f));

	can_get_interrupted = true;
	action_cooldown = 0.3f;

	type = EnemyType::CIRI_CLONE;

	Boss::StartEnemy();

	meshes = game_object->GetChild("Meshes");

	fight_controller = GameObject::FindWithName("Ciri")->GetComponent<CiriFightController>();

	state = Boss::BossState::NONE;
	animator->PlayState("Spawn");
}

void Ciri::UpdateEnemy()
{
	Boss::UpdateEnemy();
}

void Ciri::CleanUpEnemy()
{
	Boss::CleanUpEnemy();
}

float Ciri::GetDamaged(float dmg, PlayerController* player, float3 knock_back)
{

	if (can_get_interrupted) {
		animator->PlayState("Hit");
		if (current_action)current_action->state = Boss::ActionState::ENDED;
		else
			LOG("ASDSADASF");
		SetIdleState();
	}

	return Boss::GetDamaged(dmg, player);
}

void Ciri::SetActionProbabilities()
{
	Boss::SetActionProbabilities();


	if (player_distance[0] <= mini_scream_range && player_distance[1] <= mini_scream_range && fight_controller->can_mini_scream) {
		actions.find("MiniScream")->second->probability = 100.0f;
		action_cooldown = 0.5f;
		return;
	}else if (player_distance[0] <= combo_range || player_distance[1] <= combo_range) {
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

void Ciri::LaunchDashAction()
{
	//dash_direction = (player_controllers[player_distance[0] > player_distance[1] ? 1 : 0]->game_object->transform->GetGlobalPosition() - this->transform->GetGlobalPosition()).Normalized();
	//target = player_distance[0] > player_distance[1] ? 1 : 0;
	target = Random::GetRandomIntBetweenTwo(0, 1);
	dash_direction = (player_controllers[target]->game_object->transform->GetGlobalPosition() - this->transform->GetGlobalPosition()).Normalized();
	animator->PlayState("Dash");

	OrientToPlayerWithoutSlerp(target);
}

void Ciri::LaunchComboAction()
{
	animator->PlayState("Combo");
	target = player_distance[0] > player_distance[1] ? 1 : 0;
}

void Ciri::LaunchMiniScreamAction()
{
	fight_controller->can_mini_scream = false;
	animator->PlayState("Scream");
}

void Ciri::MiniScream()
{
	if (player_distance[0] <= mini_scream_range) {
		float3 knockbak_direction = (player_controllers[0]->transform->GetGlobalPosition() - this->transform->GetGlobalPosition()).Normalized();
		player_controllers[0]->ReceiveDamage(mini_scream_damage, knockbak_direction * mini_scream_force);
	}

	if (player_distance[1] <= mini_scream_range) {
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

	return current_action->state;
}

Boss::ActionState Ciri::UpdateMiniScreamAction()
{
	LOG("UPDATING MiniScream ACTION");

	return current_action->state;
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
		can_get_interrupted = true;
		ReleaseParticle("EnemyAttackParticle");
		EndComboAction(nullptr);
	}
	if (strcmp(name, "Combo") == 0) {
		can_get_interrupted = true;
		ReleaseParticle("EnemyAttackParticle");
	}
	if (strcmp(name, "Combo2") == 0) {
		can_get_interrupted = true;
		ReleaseParticle("EnemyAttackParticle");
	}
	if (strcmp(name, "Scream") == 0) {
		current_action->state = Boss::ActionState::ENDED;
	}
	if (strcmp(name, "Spawn") == 0) {
		state = Boss::BossState::IDLE;
	}
}

void Ciri::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "PlayerAttack") == 0) {
		PlayerController* player = collider->game_object_attached->GetComponentInParent<PlayerController>();
		if (player && player->attacks->GetCurrentAttack()->CanHit(this))
		{
			float dmg_received = player->attacks->GetCurrentDMG();
			float3 knock = (this->transform->GetGlobalPosition() - player->game_object->transform->GetGlobalPosition()).Normalized();
			knock = knock * player->attacks->GetCurrentAttack()->info.stats["KnockBack"].GetValue();

			player->OnHit(this, GetDamaged(dmg_received, player, knock));

			HitFreeze(player->attacks->GetCurrentAttack()->info.freeze_time);
		}
	}
}

void Ciri::OnDrawGizmosSelected()
{
	Gizmos::DrawWireSphere(transform->GetGlobalPosition(), mini_scream_range, Color::Cyan()); //snap_range
	Gizmos::DrawWireSphere(transform->GetGlobalPosition(), scream_range, Color::Cyan()); //snap_range
}
