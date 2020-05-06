#include "GameManager.h"
#include "PlayerManager.h"
#include "EnemyManager.h"
#include "PlayerController.h"
#include "CiriFightController.h"
#include "CiriOriginal.h"


void CiriOriginal::StartEnemy()
{
	actions.emplace("RockThrow", new BossAction(ActionType::ROCKTHROW, 0.0f));
	actions.emplace("Scream", new BossAction(ActionType::SCREAM, 0.0f));

	can_get_interrupted = true;
	action_cooldown = 0.0f;

	type = EnemyType::CIRI;

	Boss::StartEnemy();

	state = Boss::BossState::NONE;

	meshes = game_object->GetChild("Meshes");

	fight_controller = GetComponent<CiriFightController>();
}

void CiriOriginal::UpdateEnemy()
{
	Boss::UpdateEnemy();
}

void CiriOriginal::CleanUpEnemy()
{
	Boss::CleanUpEnemy();
}

float CiriOriginal::GetDamaged(float dmg, PlayerController* player, float3 knock_back)
{
	return Boss::GetDamaged(dmg, player);
}

void CiriOriginal::SetActionProbabilities()
{
	Boss::SetActionProbabilities();

	if (fight_controller->phase_change) {
		actions.find("Scream")->second->probability = 100.0f;
	}
	else if (fight_controller->phase == 2) {
		action_cooldown = 10.0f;
		actions.find("RockThrow")->second->probability = 100.0f;
	}
}

void CiriOriginal::SetActionVariables()
{
	Boss::SetActionVariables();
}

bool CiriOriginal::IsOnAction()
{
	return current_action != nullptr;
}

void CiriOriginal::LaunchAction()
{
	Boss::LaunchAction();

	switch (current_action->type)
	{
	case CiriOriginal::ActionType::NONE:
		break;
	case CiriOriginal::ActionType::ROCKTHROW:
		LaunchRockAction();
		break;
	case CiriOriginal::ActionType::SCREAM:
		LaunchScreamAction();
		break;
	default:
		break;
	}

	current_action->state = ActionState::UPDATING;
}

void CiriOriginal::Scream()
{
	if (player_distance[0] <= scream_range) {
		float3 knockbak_direction = (player_controllers[0]->transform->GetGlobalPosition() - this->transform->GetGlobalPosition()).Normalized();
		player_controllers[0]->ReceiveDamage(0, knockbak_direction * scream_force);
	}

	if (player_distance[1] <= scream_range) {
		float3 knockbak_direction = (player_controllers[1]->transform->GetGlobalPosition() - this->transform->GetGlobalPosition()).Normalized();
		player_controllers[1]->ReceiveDamage(0, knockbak_direction * scream_force);

	}
}

void CiriOriginal::LaunchRockAction()
{
	int target = Random::GetRandomIntBetweenTwo(0, 1);
	float3 throw_direction = (player_controllers[target]->transform->GetGlobalPosition() - this->transform->GetGlobalPosition()).Normalized();
	float distance_force_factor = 0.0f;
	distance_force_factor = transform->GetGlobalPosition().Distance(player_controllers[target]->transform->GetGlobalPosition()) * rock_force;
	GameObject* rock_ = GameObject::Instantiate(rock, float3(transform->GetGlobalPosition().x, transform->GetGlobalPosition().y + 5.0f, transform->GetGlobalPosition().z));
	throw_direction.y = 0;
	rock_->GetComponent<ComponentRigidBody>()->AddForce(throw_direction * distance_force_factor);
}

void CiriOriginal::LaunchScreamAction()
{
	animator->PlayState("Scream");
}

Boss::ActionState CiriOriginal::UpdateAction()
{
	switch (current_action->type)
	{
	case CiriOriginal::ActionType::NONE:
		break;
	case CiriOriginal::ActionType::ROCKTHROW:
		current_action->state = ActionState::ENDED;
		break;
	case CiriOriginal::ActionType::SCREAM:
		current_action->state = ActionState::ENDED;
		fight_controller->phase_change = false;
		break;
	default:
		break;
	}

	return current_action->state;
}

void CiriOriginal::EndAction(GameObject* go_ended)
{
	switch (current_action->type)
	{
	case CiriOriginal::ActionType::NONE:
		break;
	case CiriOriginal::ActionType::ROCKTHROW:
		break;
	case CiriOriginal::ActionType::SCREAM:
		break;
	default:
		break;
	}
}

void CiriOriginal::OnAnimationEnd(const char* name)
{
}

void CiriOriginal::OnDrawGizmosSelected()
{

}
