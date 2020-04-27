#include "GameManager.h"
#include "PlayerManager.h"
#include "EnemyManager.h"
#include "PlayerController.h"
#include "Boss.h"



Boss::BossAction::BossAction(ActionType _type, float _probability)
{
	type = _type;
	probability = _probability;
}

void Boss::StartEnemy()
{
	Enemy::StartEnemy();
}

void Boss::UpdateEnemy()
{
	switch (state)
	{
	case Enemy::EnemyState::NONE:
		break;
	case Enemy::EnemyState::IDLE:
		if (player_distance[0] < stats["VisionRange"].GetValue() || player_distance[1] < stats["VisionRange"].GetValue()) {
			if (time_to_action <= action_cooldown)
				time_to_action += Time::GetDT();
			else {
				SetAttackState();
			}
		}
		break;
	case Enemy::EnemyState::ATTACK:
		if (current_action) {
			if (UpdateAction() == ActionState::ENDED) {
				SetIdleState();
			}
		}
		else
			LOG("NO CURRENT ACTION DETECTED");
		break;
	case Enemy::EnemyState::HIT:
		if (current_action)
			state = EnemyState::ATTACK;
		else
			state = EnemyState::IDLE;
		break;
	case Enemy::EnemyState::DYING: {
		EnemyManager* enemy_manager = GameObject::FindWithName("GameManager")->GetComponent< EnemyManager>();
		Invoke([enemy_manager, this]() -> void {enemy_manager->DeleteEnemy(this); }, 5);
		state = EnemyState::DEAD;
	}
								 break;
	case Enemy::EnemyState::DEAD:
		break;
	default:
		break;
	}
}

void Boss::CleanUpEnemy()
{
	for (auto it = actions.begin(); it != actions.end(); ++it) {
		delete (*it).second;
	}

	current_action = nullptr;
	delete current_action;
}

float Boss::GetDamaged(float dmg, PlayerController* player)
{
	return Enemy::GetDamaged(dmg, player);
}

void Boss::SetActionVariables()
{
	player_distance[0] = 0;
	player_distance[1] = 0;

	player_distance[0] = transform->GetGlobalPosition().Distance(player_controllers[0]->game_object->transform->GetGlobalPosition());
	player_distance[1] = transform->GetGlobalPosition().Distance(player_controllers[1]->game_object->transform->GetGlobalPosition());
}

void Boss::SetActionProbabilities()
{
	for (auto it = actions.begin(); it != actions.end(); ++it) {
		(*it).second->probability = 0.f;
	}
}

void Boss::SelectAction()
{
	float rand_num = rand() % 100 + 1;
	float aux = 0.f;

	current_action = actions["Root"];

	for (auto it = actions.begin(); it != actions.end(); ++it) {
		if (rand_num > aux&& rand_num <= (aux + (*it).second->probability)) {
			current_action = (*it).second;
			break;
		}
		aux = (aux + (*it).second->probability);
	}
}

void Boss::SetIdleState()
{
	current_action = nullptr;
	state = Enemy::EnemyState::IDLE;
}

void Boss::SetAttackState()
{
	SetActionVariables();
	SetActionProbabilities();
	SelectAction();
	time_to_action = 0.0f;
	state = Enemy::EnemyState::ATTACK;
	current_action->state = ActionState::LAUNCH;
	LaunchAction();
}

bool Boss::IsOnAction()
{
	return current_action->state != Boss::ActionState::NONE;
}

void Boss::LaunchAction()
{
	rotate_time = 0.0f;
}

Boss::ActionState Boss::UpdateAction()
{
	return ActionState();
}

void Boss::EndAction(GameObject* go_ended)
{
}

void Boss::SetStats(const char* json)
{
	Enemy::SetStats(json);
}

void Boss::OrientToPlayer(int target)
{
	direction = -(player_controllers[target]->transform->GetGlobalPosition() - transform->GetLocalPosition()).Normalized();
	float desired_angle = atan2f(direction.z, direction.x);
	desired_angle = -(desired_angle * Maths::Rad2Deg() + 90.f) * Maths::Deg2Rad();
	Quat rot = Quat::RotateAxisAngle(float3::unitY(), desired_angle);
	Quat current_rot = Quat::Slerp(transform->GetGlobalRotation(), rot, rotate_time);
	transform->SetGlobalRotation(current_rot);
	if(rotate_time < time_to_rotate)
		rotate_time += Time::GetDT();
}
