#include "GameManager.h"

#include "ParticlePool.h"
#include "PlayerManager.h"
#include "PlayerController.h"
#include "Leshen.h"
#include "RootLeshen.h"

RootLeshen::RootLeshen() : Alien()
{
}

RootLeshen::~RootLeshen()
{
}

void RootLeshen::Reset()
{
	root_speed = 0.005f;
	rooted_effect_direction = { 0.0f, 1.0f, 0.0f };
	life_time = 0.0f;
	root_time = 0.0f;
	total_life_time = 5.0f;
	total_root_time = 0.0f;
	root_time_distance_factor = 0.08f;
	base_total_root_time = 2.0f;

	state = ROOTSTATE::SEEK;
	game_object->GetComponent<ComponentAnimator>()->PlayState("Idle");
	game_object->GetComponent<ComponentCollider>()->SetEnable(false);
}

void RootLeshen::Start()
{
	root_speed = 0.005f;
	rooted_effect_direction = { 0.0f, 1.0f, 0.0f };
	life_time = 0.0f;
	root_time = 0.0f;
	total_life_time = 5.0f;
	total_root_time = 0.0f;
	root_time_distance_factor = 0.08f;
	base_total_root_time = 2.0f;

	state = ROOTSTATE::SEEK;
	game_object->GetComponent<ComponentAnimator>()->PlayState("Idle");
}

void RootLeshen::Update()
{
	if (state == ROOTSTATE::SEEK) {
		direction = (GameManager::instance->player_manager->players[target]->transform->GetGlobalPosition() - transform->GetGlobalPosition()).Normalized();
		direction.y = 0;
		transform->AddPosition(direction * speed);
		float angle = atan2f(direction.z, direction.x);
		Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() + 180) * Maths::Deg2Rad());
		transform->SetGlobalRotation(rot);

		if (life_time <= total_life_time) {
			life_time += Time::GetDT();
		}
		else {
			GameManager::instance->particle_pool->ReleaseInstance("Leshen_roots_attack", game_object);
			leshen->current_action->state = Leshen::ActionState::ENDED;
			Reset();
		}
	}
	else if (state == ROOTSTATE::ROOT) {
		//transform->AddPosition(rooted_effect_direction * root_speed);
		if (root_time <= total_root_time)
			root_time += Time::GetDT();
		else {
			GameManager::instance->particle_pool->ReleaseInstance("Leshen_roots_attack", game_object);
			Reset();
		}
	}
}

void RootLeshen::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0) {
		PlayerController* player_ctrl = collider->game_object_attached->GetComponent<PlayerController>();
		if (player_ctrl && !player_ctrl->is_immune && player_ctrl->can_move && state != ROOTSTATE::ROOT) {
			state = ROOTSTATE::ROOT;
			transform->SetGlobalPosition(collider->game_object_attached->transform->GetGlobalPosition());
			leshen->EndAction(game_object);
			game_object->SetNewParent(player_ctrl->game_object);
			game_object->transform->SetLocalPosition(float3::zero());
			game_object->GetComponent<ComponentAnimator>()->PlayState("Root");
			float distance = leshen->transform->GetGlobalPosition().Distance(leshen->player_controllers[target]->transform->GetGlobalPosition());
			total_root_time = base_total_root_time + (distance * root_time_distance_factor);
			player_ctrl->ApplyRoot(total_root_time);
		}
		else {
			LOG("There's no Player Controller in GO in ArrowScript!");
		}
	}
	//else if (strcmp(collider->game_object_attached->GetTag(), "Enemy") != 0)
	//{
	//	GameObject::Destroy(game_object);
	//}
}