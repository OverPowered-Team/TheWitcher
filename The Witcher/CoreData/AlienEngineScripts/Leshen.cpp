#include "GameManager.h"
#include "PlayerManager.h"
#include "EnemyManager.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"
#include "RootLeshen.h"
#include "CrowsLeshen.h"
#include "Leshen.h"
#include "Scores_Data.h"
#include "Boss_Lifebar.h"

void Leshen::StartEnemy()
{
	actions.emplace("Root", new BossAction(ActionType::ROOT, 33.0f));
	actions.emplace("Melee", new BossAction(ActionType::MELEE, 33.0f));
	actions.emplace("Crows", new BossAction(ActionType::CROWS, 34.0f));
	actions.emplace("Cloud", new BossAction(ActionType::CLOUD, .0f));
	
	can_get_interrupted = false;

	type = EnemyType::LESHEN;

	Boss::StartEnemy();

	HUD = GameObject::FindWithName("Boss_HUD")->GetComponent<Boss_Lifebar>();

	meshes = game_object->GetChild("Meshes");
	cloud_collider = game_object->GetChild("CloudCollider");
	cloud_collider->GetComponent<ComponentSphereCollider>()->SetEnable(false);

	initial_position = transform->GetGlobalPosition();
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
	float damage = Boss::GetDamaged(dmg, player);

	if (stats["Health"].GetValue() <= 0.f) {
		state = Boss::BossState::DYING;
		animator->PlayState("Death");
		Scores_Data::won_level1 = true;
		Scores_Data::last_scene = SceneManager::GetCurrentScene();
		Scores_Data::player1_kills = GameObject::FindWithName("GameManager")->GetComponent<GameManager>()->player_manager->players[0]->player_data.total_kills;
		Scores_Data::player2_kills = GameObject::FindWithName("GameManager")->GetComponent<GameManager>()->player_manager->players[1]->player_data.total_kills;
		Invoke(std::bind(&Leshen::ChangeScene, this), 4.f);
	}

	return damage;
}

void Leshen::SetActionProbabilities()
{
	Boss::SetActionProbabilities();

	if (times_hitted >= 10) {
		actions.find("Cloud")->second->probability = 100.0f;
		return;
	}
	else if (player_distance[0] <= melee_range && player_controllers[0]->state->type != StateType::DEAD || player_distance[1] <= melee_range && player_controllers[1]->state->type != StateType::DEAD) {
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
	if (player_controllers[0]->state->type != StateType::DEAD) {
		root_1 = GameObject::Instantiate(root_prefab, this->transform->GetGlobalPosition());
		root_1->GetComponent<RootLeshen>()->leshen = this;
		root_1->GetComponent<RootLeshen>()->target = 0;
	}

	if (player_controllers[1]->state->type != StateType::DEAD) {
		root_2 = GameObject::Instantiate(root_prefab, this->transform->GetGlobalPosition());
		root_2->GetComponent<RootLeshen>()->leshen = this;
		root_2->GetComponent<RootLeshen>()->target = 1;
	}
}

void Leshen::LaunchMeleeAction()
{

}

void Leshen::LaunchCrowsAction()
{
	crows = GameObject::Instantiate(crow_prefab, float3(transform->GetGlobalPosition().x, transform->GetGlobalPosition().y + 0.5f, transform->GetGlobalPosition().z), true);
	if (player_rooted[0] && player_controllers[0]->state->type != StateType::DEAD) {
		crows->GetComponent<CrowsLeshen>()->target = 0;
		crows_target = 0;
	}
	else if (player_rooted[1] && player_controllers[1]->state->type != StateType::DEAD){
		crows->GetComponent<CrowsLeshen>()->target = 1;
		crows_target = 1;
	}
	else if(player_controllers[0]->state->type != StateType::DEAD && player_controllers[1]->state->type != StateType::DEAD){
		crows->GetComponent<CrowsLeshen>()->target = Random::GetRandomIntBetweenTwo(0, 1);
		crows_target = crows->GetComponent<CrowsLeshen>()->target;
	}
	else if (player_controllers[0]->state->type != StateType::DEAD) {
		crows->GetComponent<CrowsLeshen>()->target = 0;
		crows_target = 0;
	}
	else if (player_controllers[1]->state->type != StateType::DEAD) {
		crows->GetComponent<CrowsLeshen>()->target = 1;
		crows_target = 1;
	}

	crows->GetComponent<CrowsLeshen>()->leshen = this;
}

void Leshen::LaunchCloudAction()
{
	times_hitted = 0;
	direction = -(player_controllers[0]->transform->GetGlobalPosition() - transform->GetLocalPosition()).Normalized();
	meshes->SetEnable(false);
	SpawnParticle("Cloud", float3::zero(), true);
	game_object->GetComponent<ComponentAudioEmitter>()->StartSound("Play_Leshen_Cloud_Appears");
	cloud_collider->GetComponent<ComponentSphereCollider>()->SetEnable(true);
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

	if (player_distance[0] < player_distance[1] && player_controllers[0]->state->type != StateType::DEAD)
		OrientToPlayer(0);
	else if(player_controllers[1]->state->type != StateType::DEAD){
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

	float distance_to_initial = initial_position.Distance(transform->GetGlobalPosition());

	if (times_switched < total_switch_times) {
		if (direction_time <= switch_direction_time) {
			if (distance_to_initial > cloud_max_distance)
				direction = -direction;
			character_ctrl->Move(direction * speed * Time::GetDT());
			direction_time += Time::GetDT();
		}
		else {
			SetRandomDirection();
			times_switched++;
			direction_time = 0;
			LaunchCrowsAction();
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
	cloud_collider->GetComponent<ComponentSphereCollider>()->SetEnable(false);
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

void Leshen::SetStats(const char* json)
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

void Leshen::ChangeScene()
{
	SceneManager::LoadScene("Wagonnetes", FadeToBlackType::FADE);
}
