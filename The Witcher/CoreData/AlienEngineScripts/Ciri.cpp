#include "GameManager.h"
#include "PlayerManager.h"
#include "EnemyManager.h"
#include "PlayerController.h"
#include "Ciri.h"


void Ciri::StartEnemy()
{
	actions.emplace("Dash", new BossAction(ActionType::DASH, 0.0f));
	actions.emplace("Combo", new BossAction(ActionType::COMBO, 0.0f));
	actions.emplace("MiniScream", new BossAction(ActionType::MINISCREAM, 0.0f));
	actions.emplace("Scream", new BossAction(ActionType::SCREAM, 0.0f));

	can_get_interrupted = true;
	action_cooldown = 0.3f;

	type = EnemyType::CIRI;

	Boss::StartEnemy();

	meshes = game_object->GetChild("Meshes");
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
	return Boss::GetDamaged(dmg, player);
}

void Ciri::SetActionProbabilities()
{
	Boss::SetActionProbabilities();


	if (player_distance[0] <= mini_scream_range && player_distance[1] <= mini_scream_range) {
		actions.find("MiniScream")->second->probability = 100.0f;
		action_cooldown = 3.0f;
		return;
	}else if (player_distance[0] <= combo_range || player_distance[1] <= combo_range) {
		actions.find("Combo")->second->probability = 100.0f;
		action_cooldown = 3.0f;
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
	dash_direction = (player_controllers[Random::GetRandomIntBetweenTwo(0, 1)]->game_object->transform->GetGlobalPosition() - this->transform->GetGlobalPosition()).Normalized();
}

void Ciri::LaunchComboAction()
{
}

void Ciri::LaunchMiniScreamAction()
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
			dash_timer = 0;
			current_action->state = Boss::ActionState::ENDED;
		}
	}
	else {
		dash_timer = 0;
		current_action->state = Boss::ActionState::ENDED;
	}

	return current_action->state;
}

Boss::ActionState Ciri::UpdateComboAction()
{
	LOG("UPDATING COMBO ACTION");

	current_action->state = Boss::ActionState::ENDED;

	return current_action->state;
}

Boss::ActionState Ciri::UpdateMiniScreamAction()
{
	LOG("UPDATING MiniScream ACTION");

	current_action->state = Boss::ActionState::ENDED;

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
}

void Ciri::EndMiniScreamAction(GameObject* go_ended)
{
}

void Ciri::OnAnimationEnd(const char* name)
{
}

void Ciri::OnDrawGizmosSelected()
{
	Gizmos::DrawWireSphere(transform->GetGlobalPosition(), mini_scream_range, Color::Cyan()); //snap_range
	Gizmos::DrawWireSphere(transform->GetGlobalPosition(), scream_range, Color::Cyan()); //snap_range
}
