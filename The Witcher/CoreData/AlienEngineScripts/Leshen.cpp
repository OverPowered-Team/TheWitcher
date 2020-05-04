#include "GameManager.h"
#include "PlayerManager.h"
#include "EnemyManager.h"
#include "PlayerController.h"
#include "RootLeshen.h"
#include "CrowsLeshen.h"
#include "Leshen.h"

void Leshen::StartEnemy()
{
	actions.emplace("Root", new BossAction(ActionType::ROOT, 33.0f));
	actions.emplace("Melee", new BossAction(ActionType::MELEE, 33.0f));
	actions.emplace("Crows", new BossAction(ActionType::CROWS, 34.0f));
	actions.emplace("Cloud", new BossAction(ActionType::CLOUD, .0f));
	
	can_get_interrupted = false;

	type = EnemyType::LESHEN;

	Boss::StartEnemy();

	meshes = game_object->GetChild("Meshes");
}

void Leshen::UpdateEnemy()
{
	Boss::UpdateEnemy();
}

void Leshen::CleanUpEnemy()
{
	Boss::CleanUpEnemy();
}

float Leshen::GetDamaged(float dmg, PlayerController* player, float3 knock)
{
	HandleHitCount();
	return Boss::GetDamaged(dmg, player);
}

void Leshen::SetActionProbabilities()
{
	Boss::SetActionProbabilities();

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

bool Leshen::IsOnAction()
{
	return current_action != nullptr;
}

void Leshen::LaunchAction()
{
	Boss::LaunchAction();

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
		break;
	case Leshen::ActionType::CLOUD:
		LaunchCloudAction();
		break;
	default:
		break;
	}

	current_action->state = ActionState::UPDATING;
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
	SpawnParticle("Cloud");
}

Boss::ActionState Leshen::UpdateAction()
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

	return current_action->state;
}

Leshen::ActionState Leshen::UpdateRootAction()
{
	LOG("UPDATING ROOT ACTION");

	return current_action->state;
}

Leshen::ActionState Leshen::UpdateMeleeAction()
{
	LOG("UPDATING MELEE ACTION");

	if (player_distance[0] < player_distance[1])
		OrientToPlayer(0);
	else {
		OrientToPlayer(1);
	}

	return current_action->state;
}

Leshen::ActionState Leshen::UpdateCrowsAction()
{
	LOG("UPDATING CROWS ACTION");

	OrientToPlayer(crows_target);

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

void Leshen::EndAction(GameObject* go_ended)
{
	switch (current_action->type)
	{
	case Leshen::ActionType::NONE:
		break;
	case Leshen::ActionType::ROOT:
		EndRootAction(go_ended);
		break;
	case Leshen::ActionType::MELEE:
		EndMeleeAction();
		break;
	case Leshen::ActionType::CROWS:
		EndCrowsAction();
		break;
	case Leshen::ActionType::CLOUD:
		EndCloudAction();
		break;
	default:
		break;
	}
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

void Leshen::EndCrowsAction()
{
	current_action->state = Leshen::ActionState::ENDED;
}

void Leshen::EndCloudAction()
{
	meshes->SetEnable(true);
	ReleaseParticle("Cloud");
	current_action->state = Leshen::ActionState::ENDED;
	direction_time = 0.0f;
	times_switched = 0;
}

void Leshen::SetActionVariables()
{
	Boss::SetActionVariables();

	player_rooted[0] = false;
	player_rooted[1] = false;	
	
	if (!player_controllers[0]->can_move) {
		player_rooted[0] = true;
	}
	else if (!player_controllers[1]->can_move) {
		player_rooted[1] = true;
	}
}

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

void Leshen::OnAnimationEnd(const char* name)
{
	if (strcmp(name, "Melee") == 0) {
		EndMeleeAction();
	}
}
