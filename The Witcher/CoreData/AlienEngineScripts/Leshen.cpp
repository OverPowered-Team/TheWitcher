#include "Leshen.h"

void Leshen::StartEnemy()
{
	actions.emplace("Root", new LeshenAction(ActionType::ROOT, 33.0f));
	actions.emplace("Melee", new LeshenAction(ActionType::MELEE, 33.0f));
	actions.emplace("Crows", new LeshenAction(ActionType::CROWS, 34.0f));
	actions.emplace("Whip", new LeshenAction(ActionType::WHIP, .0f));
	actions.emplace("Cloud", new LeshenAction(ActionType::CLOUD, .0f));

	Enemy::StartEnemy();
}

void Leshen::UpdateEnemy()
{
	switch (state)
	{
	case Enemy::EnemyState::NONE:
		break;
	case Enemy::EnemyState::IDLE:
		if (time_to_action < action_cooldown)
			time_to_action += Time::GetDT();
		else {
			SetAttackState();
		}
		break;
	case Enemy::EnemyState::ATTACK:
		if (current_action) {
			if (!UpdateAction()) {
				SetIdleState();
			}
		}
		else
			LOG("NO CURRENT ACTION DETECTED");
		break;
	case Enemy::EnemyState::HIT:
		break;
	case Enemy::EnemyState::DYING:
		break;
	case Enemy::EnemyState::DEAD:
		break;
	default:
		break;
	}
}

void Leshen::CleanUpEnemy()
{
	for (auto it = actions.begin(); it != actions.end(); ++it) {
		delete (*it).second;
	}

	current_action = nullptr;
	delete current_action;
}

void Leshen::SetStats(const char* json)
{
}

void Leshen::SetActionProbabilities()
{
	for (auto it = actions.begin(); it != actions.end(); ++it) {
		(*it).second->probability = 0.f;
	}

	if (phase == 1) {
		if (player_distance[0] <= melee_range || player_distance[1] <= melee_range) {
			actions.find("Melee")->second->probability = 100.0f;
			return;
		}
		else if (player_rooted[0] || player_rooted[1]) {
			actions.find("Crows")->second->probability = 100.0f;
			return;
		}
		else {
			actions.find("Crows")->second->probability = 50.0f;
			actions.find("Root")->second->probability = 50.0f;
		}
	}
	else if (phase == 2) {
		if (times_hitted == 3) {
			actions.find("Cloud")->second->probability = 100.0f;
			return;
		}
		else if (player_rooted[0] || player_rooted[1]) {
			actions.find("Whip")->second->probability = 100.0f;
			return;
		}
		else {
			actions.find("Whip")->second->probability = 20.0f;
			actions.find("Root")->second->probability = 80.0f;
		}
	}

}

void Leshen::SelectAction()
{

	float rand_num = (float)rand() / RAND_MAX; // 0.4
	float aux = 0.f;

	for (auto it = actions.begin(); it != actions.end(); ++it) {
		if (rand_num > aux&& rand_num <= (aux + (*it).second->probability) * 0.01) {
			current_action = (*it).second;
			break;
		}
		aux = (aux + (*it).second->probability * 0.01) + 0.01;
	}

}

bool Leshen::IsOnAction()
{
	return current_action != nullptr;
}

void Leshen::SetIdleState()
{
	current_action = nullptr;
	state = Enemy::EnemyState::IDLE;
}

void Leshen::SetAttackState()
{
	SetActionProbabilities();
	SelectAction();
	time_to_action = 0.0f;
	state = Enemy::EnemyState::ATTACK;
	//animator->PlayState("Action")
}

Leshen::LeshenAction::LeshenAction(ActionType _type, float _probability)
{
	type = _type;
	probability = _probability;
}

bool Leshen::UpdateAction()
{
	bool updating = true;

	switch (current_action->type)
	{
	case Leshen::ActionType::NONE:
		break;
	case Leshen::ActionType::ROOT:
		updating = UpdateRootAction();
		break;
	case Leshen::ActionType::MELEE:
		updating = UpdateMeleeAction();
		break;
	case Leshen::ActionType::CROWS:
		updating = UpdateCrowsAction();
		break;
	case Leshen::ActionType::WHIP:
		updating = UpdateWhipAction();
		break;
	case Leshen::ActionType::CLOUD:
		updating = UpdateCloudAction();
		break;
	default:
		break;
	}

	return updating;
}

bool Leshen::UpdateRootAction()
{
	bool updating = true;

	LOG("UPDATING ROOT ACTION");

	action_time += Time::GetDT();

	if (action_time >= 3.0f) {
		updating = false;
		action_time = 0.0f;
	}

	return updating;
}

bool Leshen::UpdateMeleeAction()
{
	bool updating = true;

	LOG("UPDATING MELEE ACTION");

	action_time += Time::GetDT();

	if (action_time >= 3.0f) {
		updating = false;
		action_time = 0.0f;
	}

	return updating;
}

bool Leshen::UpdateCrowsAction()
{
	bool updating = true;

	LOG("UPDATING MELEE ACTION");

	action_time += Time::GetDT();

	if (action_time >= 3.0f) {
		updating = false;
		action_time = 0.0f;
	}

	return updating;
}

bool Leshen::UpdateWhipAction()
{
	bool updating = true;

	action_time += Time::GetDT();

	if (action_time >= 3.0f) {
		updating = false;
		action_time = 0.0f;
	}

	return updating;
}

bool Leshen::UpdateCloudAction()
{
	bool updating = true;

	action_time += Time::GetDT();

	if (action_time >= 3.0f) {
		updating = false;
		action_time = 0.0f;
	}

	return updating;
}

void Leshen::SetActionVariables()
{
}

void Leshen::ChangePhase()
{
	phase = 2;
}

void Leshen::HandleHitCount()
{
	if (phase == 2)
	{
		if (times_hitted < 3)
			times_hitted++;
		else
			times_hitted = 0;
	}
}
