#include "GameManager.h"
#include "PlayerManager.h"
#include "EnemyManager.h"
#include "PlayerController.h"
#include "RootLeshen.h"
#include "CrowsLeshen.h"
#include "Leshen.h"

void Leshen::StartEnemy()
{
	actions.emplace("Root", new LeshenAction(ActionType::ROOT, 33.0f));
	actions.emplace("Melee", new LeshenAction(ActionType::MELEE, 33.0f));
	actions.emplace("Crows", new LeshenAction(ActionType::CROWS, 34.0f));
	actions.emplace("Cloud", new LeshenAction(ActionType::CLOUD, .0f));
	
	can_get_interrupted = false;

	type = EnemyType::LESHEN;

	Enemy::StartEnemy();

	meshes = game_object->GetChild("Meshes");
}

void Leshen::UpdateEnemy()
{
	player_distance[0] = transform->GetGlobalPosition().Distance(player_controllers[0]->game_object->transform->GetGlobalPosition());

	//switch (state)
	//{
	//case Enemy::EnemyState::NONE:
	//	break;
	//case Enemy::EnemyState::IDLE:
	//	if (player_distance[0] < stats["VisionRange"].GetValue()) {
	//		if (time_to_action <= action_cooldown)
	//			time_to_action += Time::GetDT();
	//		else {
	//			SetAttackState();
	//		}
	//	}
	//	break;
	//case Enemy::EnemyState::ATTACK:
	//	if (current_action) {
	//		if (!UpdateAction()) {
	//			SetIdleState();
	//		}
	//	}
	//	else
	//		LOG("NO CURRENT ACTION DETECTED");
	//	break;
	//case Enemy::EnemyState::HIT:
	//	if (current_action)
	//		state = EnemyState::ATTACK;
	//	else
	//		state = EnemyState::IDLE;
	//	break;
	//case Enemy::EnemyState::DYING: {
	//	EnemyManager* enemy_manager = GameObject::FindWithName("GameManager")->GetComponent< EnemyManager>();
	//	//Ori Ori function sintaxis
	//	Invoke([enemy_manager, this]() -> void {enemy_manager->DeleteEnemy(this); }, 5);
	//	audio_emitter->StartSound("SoldierDeath");
	//	state = EnemyState::DEAD;
	//}
	//	break;
	//case Enemy::EnemyState::DEAD:
	//	break;
	//default:
	//	break;
	//}
}

void Leshen::CleanUpEnemy()
{
	for (auto it = actions.begin(); it != actions.end(); ++it) {
		delete (*it).second;
	}

	current_action = nullptr;
	delete current_action;
}

float Leshen::GetDamaged(float dmg, PlayerController* player)
{
	HandleHitCount();
	LOG("health remaining %f", stats["Health"].GetValue());
	LOG("hitcount %i", times_hitted);
	return Enemy::GetDamaged(dmg, player);
}

void Leshen::OrientToPlayer(int target)
{
	direction = -(player_controllers[target]->transform->GetGlobalPosition() - transform->GetLocalPosition()).Normalized();
	float angle = atan2f(direction.z, direction.x);
	Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() + 90.f) * Maths::Deg2Rad());
	transform->SetGlobalRotation(rot);
}

void Leshen::SetStats(const char* json)
{
	Enemy::SetStats(json);
}

void Leshen::SetActionProbabilities()
{
	for (auto it = actions.begin(); it != actions.end(); ++it) {
		(*it).second->probability = 0.f;
	}

	if (times_hitted >= 10) {
		actions.find("Cloud")->second->probability = 100.0f;
		return;
	}
	else if (player_distance[0] <= melee_range || player_distance[1] <= melee_range) {
		actions.find("Melee")->second->probability = 100.0f;
		return;
	}
	else if (player_rooted[0] || player_rooted[1]) {
		actions.find("Crows")->second->probability = 100.0f;
		return;
	}
	else {
		actions.find("Crows")->second->probability = 40.0f;
		actions.find("Root")->second->probability = 60.0f;
	}
	

}

void Leshen::SelectAction()
{

	float rand_num = rand() % 100 + 1;
	float aux = 0.f;

	current_action = actions["Root"];

	for (auto it = actions.begin(); it != actions.end(); ++it) {
		if (rand_num > aux && rand_num <= (aux + (*it).second->probability)) {
			current_action = (*it).second;
			break;
		}
		aux = (aux + (*it).second->probability);
	}
}

bool Leshen::IsOnAction()
{
	return current_action != nullptr;
}

void Leshen::FinishAttack()
{

}

void Leshen::SetIdleState()
{
	/*current_action = nullptr;
	state = Enemy::EnemyState::IDLE;*/
}

void Leshen::SetAttackState()
{
	SetActionVariables();
	SetActionProbabilities();
	SelectAction();
	time_to_action = 0.0f;
	//state = Enemy::EnemyState::ATTACK;
	current_action->state = ActionState::LAUNCH;
	LaunchAction();
	//animator->PlayState("Action")
}

void Leshen::LaunchAction()
{
	switch (current_action->type)
	{
	case Leshen::ActionType::NONE:
		break;
	case Leshen::ActionType::ROOT:
		animator->PlayState("Roots");
		break;
	case Leshen::ActionType::MELEE:
		LaunchMeleeAction();
		animator->PlayState("Melee");
		break;
	case Leshen::ActionType::CROWS:
		animator->PlayState("Crows");
		if (player_rooted[0]) {
			crows_target = 0;
		}
		else if (player_rooted[1]) {
			crows_target = 1;
		}
		else {
			crows_target = rand() % 1;
		}
		OrientToPlayer(crows_target);
		break;
	case Leshen::ActionType::CLOUD:
		LaunchCloudAction();
		break;
	default:
		break;
	}

	current_action->state = ActionState::UPDATING;
}

void Leshen::OnDeathHit()
{
}

void Leshen::LaunchRootAction()
{
	root_1 = GameObject::Instantiate(root_prefab, this->transform->GetGlobalPosition());
	root_1->GetComponent<RootLeshen>()->leshen = this;
	root_1->GetComponent<RootLeshen>()->target = 0;


	root_2 = GameObject::Instantiate(root_prefab, this->transform->GetGlobalPosition());
	root_2->GetComponent<RootLeshen>()->leshen = this;
	root_2->GetComponent<RootLeshen>()->target = 1;
}

void Leshen::LaunchMeleeAction()
{
	if (player_distance[0] < player_distance[1])
		OrientToPlayer(0);
	else {
		OrientToPlayer(1);
	}
}

void Leshen::LaunchCrowsAction()
{
	crows = GameObject::Instantiate(crow_prefab, transform->GetGlobalPosition());
	if (player_rooted[0]) {
		crows->GetComponent<CrowsLeshen>()->target = 0;
		crows_target = 0;
	}
	else if (player_rooted[1]){
		crows->GetComponent<CrowsLeshen>()->target = 1;
		crows_target = 1;
	}
	else {
		crows->GetComponent<CrowsLeshen>()->target = rand() % 1;
		crows_target = crows->GetComponent<CrowsLeshen>()->target;
	}

	crows->GetComponent<CrowsLeshen>()->leshen = this;
}

void Leshen::LaunchCloudAction()
{
	times_hitted = 0;
	direction = -(player_controllers[0]->transform->GetGlobalPosition() - transform->GetLocalPosition()).Normalized();
	meshes->SetEnable(false);
	particles["Cloud"]->game_object_attached->SetEnable(true);
}

Leshen::LeshenAction::LeshenAction(ActionType _type, float _probability)
{
	type = _type;
	probability = _probability;
}

bool Leshen::UpdateAction()
{
	switch (current_action->type)
	{
	case Leshen::ActionType::NONE:
		break;
	case Leshen::ActionType::ROOT:
		current_action->state = UpdateRootAction();
		break;
	case Leshen::ActionType::MELEE:
		current_action->state = UpdateMeleeAction();
		break;
	case Leshen::ActionType::CROWS:
		current_action->state = UpdateCrowsAction();
		break;
	case Leshen::ActionType::CLOUD:
		current_action->state = UpdateCloudAction();
		break;
	default:
		break;
	}

	if (current_action->state == ActionState::ENDED)
		return false;
	else
		return true;
}

Leshen::ActionState Leshen::UpdateRootAction()
{
	LOG("UPDATING ROOT ACTION");

	return current_action->state;
}

Leshen::ActionState Leshen::UpdateMeleeAction()
{
	LOG("UPDATING MELEE ACTION");

	current_action->state = ActionState::ENDED;

	return current_action->state;
}

Leshen::ActionState Leshen::UpdateCrowsAction()
{
	LOG("UPDATING CROWS ACTION");


	return current_action->state;
}

Leshen::ActionState Leshen::UpdateCloudAction()
{
	LOG("UPDATING CLOUD ACTION");

	if (times_switched < total_switch_times) {
		if (direction_time <= switch_direction_time) {
			character_ctrl->Move(direction * speed * Time::GetDT());
			direction_time += Time::GetDT();
		}
		else {
			SetRandomDirection();
			times_switched++;
			direction_time = 0;
		}
	}
	else {
		EndCloudAction();
	}

	return current_action->state;
}

void Leshen::EndRootAction(GameObject* root)
{
	current_action->state = Leshen::ActionState::ENDED;

	if (root == root_1) {
		Destroy(root_2);
	}
	else if (root == root_2) {
		Destroy(root_1);
	}
}

void Leshen::EndMeleeAction()
{
	current_action->state = Leshen::ActionState::ENDED;
}

void Leshen::EndCrowsAction(GameObject* crow)
{
	Destroy(crow);
	current_action->state = Leshen::ActionState::ENDED;
}

void Leshen::EndCloudAction()
{
	meshes->SetEnable(true);
	particles["Cloud"]->game_object_attached->SetEnable(false);
	current_action->state = Leshen::ActionState::ENDED;
	direction_time = 0.0f;
	times_switched = 0;
}

void Leshen::SetActionVariables()
{
	player_distance[0] = 0;
	player_distance[1] = 0;

	player_distance[0] = transform->GetGlobalPosition().Distance(player_controllers[0]->game_object->transform->GetGlobalPosition());
	player_distance[1] = transform->GetGlobalPosition().Distance(player_controllers[1]->game_object->transform->GetGlobalPosition());

	player_rooted[0] = false;
	player_rooted[1] = false;	
	
	if (!player_controllers[0]->can_move) {
		player_rooted[0] = true;
	}
	else if (!player_controllers[1]->can_move) {
		player_rooted[1] = true;
	}
}

//void Leshen::ChangePhase()
//{
//	phase = 2;
//}

void Leshen::HandleHitCount()
{
	if (times_hitted < 10)
		times_hitted++;
}

void Leshen::SetRandomDirection()
{
	if (rand_x > 0) {
		rand_x = Random::GetRandomIntBetweenTwo(-1, 0);
		rand_z = -1;
	}
	else if (rand_x < 0) {
		rand_x = Random::GetRandomIntBetweenTwo(0, 1);
		rand_z = -1;
	}
	else {
		rand_x = Random::GetRandomIntBetweenTwo(-1, 1);
		rand_z = 1;
	}



	direction = float3(rand_x, 0, rand_z);
}