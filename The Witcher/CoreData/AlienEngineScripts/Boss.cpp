#include "GameManager.h"
#include "PlayerManager.h"
#include "EnemyManager.h"
#include "PlayerController.h"
#include "MusicController.h"
#include "Boss.h"


Boss::BossAction::BossAction(ActionType _type, float _probability)
{
	type = _type;
	probability = _probability;
}

void Boss::StartEnemy()
{
	Enemy::StartEnemy();

	state = BossState::IDLE;
}

void Boss::UpdateEnemy()
{
	player_distance[0] = transform->GetGlobalPosition().Distance(player_controllers[0]->game_object->transform->GetGlobalPosition());
	player_distance[1] = transform->GetGlobalPosition().Distance(player_controllers[1]->game_object->transform->GetGlobalPosition());

	switch (state)
	{
	case Boss::BossState::NONE:
		LOG("ADSF");
		break;
	case Boss::BossState::IDLE:
		if (player_distance[0] < stats["VisionRange"].GetValue() || player_distance[1] < stats["VisionRange"].GetValue()) {
			if (time_to_action <= action_cooldown)
				time_to_action += Time::GetDT();
			else {
				SetAttackState();
			}
			if (m_controller && !is_combat)
			{
				is_combat = true;
				m_controller->EnemyInSight((Enemy*)this);
			}
		}
		else {
			if (m_controller && is_combat)
			{
				is_combat = false;
				m_controller->EnemyLostSight((Enemy*)this);
			}
		}
		break;
	case Boss::BossState::ATTACK:
		if (current_action) {
			if (UpdateAction() == ActionState::ENDED) {
				SetIdleState();
			}
		}
		else
			LOG("NO CURRENT ACTION DETECTED");
		break;
	case Boss::BossState::HIT:
		if (current_action)
			state = BossState::ATTACK;
		else
			state = BossState::IDLE;
		break;
	case Boss::BossState::DYING: {
		EnemyManager* enemy_manager = GameObject::FindWithName("GameManager")->GetComponent< EnemyManager>();
		Invoke([enemy_manager, this]() -> void {enemy_manager->DeleteEnemy(this); }, 5);
		state = BossState::DEAD;
		if (m_controller && is_combat)
		{
			is_combat = false;
			m_controller->EnemyLostSight((Enemy*)this);
		}
	}
	break;
	case Boss::BossState::DEAD:
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

float Boss::GetDamaged(float dmg, PlayerController* player, float3 knock_back)
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
	state = Boss::BossState::IDLE;
}

void Boss::SetAttackState()
{
	SetActionVariables();
	SetActionProbabilities();
	SelectAction();
	if (current_action) {
		time_to_action = 0.0f;
		state = Boss::BossState::ATTACK;
		current_action->state = ActionState::LAUNCH;
		LaunchAction();
	}
	else {
		SetIdleState();
	}
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

void Boss::OrientToPlayerWithoutSlerp(int target)
{
	direction = -(player_controllers[target]->transform->GetGlobalPosition() - transform->GetLocalPosition()).Normalized();
	float desired_angle = atan2f(direction.z, direction.x);
	desired_angle = -(desired_angle * Maths::Rad2Deg() + 90.f) * Maths::Deg2Rad();
	Quat rot = Quat::RotateAxisAngle(float3::unitY(), desired_angle);
	transform->SetGlobalRotation(rot);
}
