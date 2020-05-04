#include "GameManager.h"
#include "ParticlePool.h"
#include "PlayerManager.h"
#include "EventManager.h"

#include "MiniGame_Revive.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"

#include "State.h"

State* IdleState::HandleInput(PlayerController* player)
{
	if (player->movement_input.Length() > 0)
	{
		return new RunningState();
	}
	if (!player->controller->isGrounded)
	{
		player->Fall();
		return new JumpingState();
	}

	if (Input::GetControllerTriggerLeft(player->controller_index) == 1.0
		|| Input::GetKeyDown(player->keyboard_spell)) {

		if (Input::GetControllerButtonDown(player->controller_index, player->controller_heavy_attack))
		{
			if (player->attacks->StartSpell(0))
				return new AttackingState();
		}
		else if (Input::GetControllerButtonDown(player->controller_index, player->controller_revive))
		{
			if (player->attacks->StartSpell(1))
				return new AttackingState();
		}
		else if (Input::GetControllerButtonDown(player->controller_index, player->controller_jump))
		{
			if (player->attacks->StartSpell(2))
				return new AttackingState();
		}
		else if (Input::GetControllerButtonDown(player->controller_index, player->controller_light_attack))
		{
			if (player->attacks->StartSpell(3))
				return new AttackingState();
		}
	}
	else if (Input::GetControllerButtonDown(player->controller_index, player->controller_light_attack)
		|| Input::GetKeyDown(player->keyboard_light_attack)) {
		player->attacks->StartAttack(PlayerAttacks::AttackType::LIGHT);
		return new AttackingState();
	}
	else if (Input::GetControllerButtonDown(player->controller_index, player->controller_heavy_attack)
		|| Input::GetKeyDown(player->keyboard_heavy_attack)) {
		player->attacks->StartAttack(PlayerAttacks::AttackType::HEAVY);
		return new AttackingState();
	}

	

	if (Input::GetControllerButtonDown(player->controller_index, player->controller_dash)
		|| Input::GetKeyDown(player->keyboard_dash))
	{
		return new RollingState();
	}

	if (Input::GetControllerButtonDown(player->controller_index, player->controller_jump)
		|| Input::GetKeyDown(player->keyboard_jump) && player->controller->isGrounded)
	{
		player->Jump();
		return new JumpingState();
	}

	if (Input::GetControllerButtonDown(player->controller_index, player->controller_revive)
		|| Input::GetKeyDown(player->keyboard_revive)) {
		if (player->CheckForPossibleRevive()) {
			player->player_data.speed = float3::zero();
			player->animator->SetBool("reviving", true);
			return new RevivingState();
		}
	}

	return nullptr;
}

void IdleState::Update(PlayerController* player)
{
	if (!player->controller->isGrounded)
	{
		player->Fall();
	}
}

void IdleState::OnEnter(PlayerController* player)
{
	player->player_data.speed = float3::zero();
}

void IdleState::OnExit(PlayerController* player)
{
}

State* RunningState::HandleInput(PlayerController* player)
{
	if (!player->mov_input)
	{
		return new IdleState();
	}
	if (!player->controller->isGrounded)
	{
		player->Fall();
		return new JumpingState();
	}

	if (Input::GetControllerTriggerLeft(player->controller_index) == 1.0
		|| Input::GetKeyDown(player->keyboard_spell)) {

		if (Input::GetControllerButtonDown(player->controller_index, player->controller_heavy_attack))
		{
			if(player->attacks->StartSpell(0))
				return new AttackingState();
		}
		else if (Input::GetControllerButtonDown(player->controller_index, player->controller_revive))
		{
			if (player->attacks->StartSpell(1))
				return new AttackingState();
		}
		else if (Input::GetControllerButtonDown(player->controller_index, player->controller_jump))
		{
			if (player->attacks->StartSpell(2))
				return new AttackingState();
		}
		else if (Input::GetControllerButtonDown(player->controller_index, player->controller_light_attack))
		{
			if (player->attacks->StartSpell(3))
				return new AttackingState();
		}
	}
	else if (Input::GetControllerButtonDown(player->controller_index, player->controller_light_attack)
		|| Input::GetKeyDown(player->keyboard_light_attack)) {
		player->attacks->StartAttack(PlayerAttacks::AttackType::LIGHT);
		return new AttackingState();
	}
	else if (Input::GetControllerButtonDown(player->controller_index, player->controller_heavy_attack)
		|| Input::GetKeyDown(player->keyboard_heavy_attack)) {
		player->attacks->StartAttack(PlayerAttacks::AttackType::HEAVY);
		return new AttackingState();
	}

	if (Input::GetControllerButtonDown(player->controller_index, player->controller_dash)
		|| Input::GetKeyDown(player->keyboard_dash))
	{
		return new RollingState();
	}

	if (Input::GetControllerButtonDown(player->controller_index, player->controller_jump)
		|| Input::GetKeyDown(player->keyboard_jump) && player->controller->isGrounded)
	{
		player->Jump();
		return new JumpingState();
	}
	if (Input::GetControllerButtonDown(player->controller_index, player->controller_revive)
		|| Input::GetKeyDown(player->keyboard_revive)) {
		if (player->CheckForPossibleRevive()) {
			return new RevivingState();
		}
	}

	return nullptr;
}

void RunningState::Update(PlayerController* player)
{
	if (Time::GetGameTime() - player->timer >= player->delay_footsteps) {
		player->timer = Time::GetGameTime();
		player->audio->StartSound();
	}
	player->HandleMovement();

	if (!player->controller->isGrounded)
	{
		player->Fall();
	}
}

void RunningState::OnEnter(PlayerController* player)
{
	player->SpawnParticle("p_run");
	player->audio->StartSound();
	player->timer = Time::GetGameTime();
}

void RunningState::OnExit(PlayerController* player)
{
	player->particles["p_run"]->SetEnable(false);
}

State* JumpingState::HandleInput(PlayerController* player)
{
	if (player->controller->isGrounded)
	{
		if (!player->mov_input)
		{
			return new IdleState();
		}
		if (player->mov_input)
		{
			return new RunningState();
		}
	}

	return nullptr;
}

void JumpingState::Update(PlayerController* player)
{
	player->HandleMovement();
}

void JumpingState::OnEnter(PlayerController* player)
{
	player->animator->SetBool("air", true);
}

void JumpingState::OnExit(PlayerController* player)
{
	player->animator->SetBool("air", false);
}

State* AttackingState::HandleInput(PlayerController* player)
{
	if (Input::GetControllerTriggerLeft(player->controller_index) == 1.0
		|| Input::GetKeyDown(player->keyboard_spell)) {

		if (Input::GetControllerButtonDown(player->controller_index, player->controller_heavy_attack))
		{
			player->attacks->ReceiveInput(PlayerAttacks::AttackType::SPELL, 0);
		}
		else if (Input::GetControllerButtonDown(player->controller_index, player->controller_revive))
		{
			player->attacks->ReceiveInput(PlayerAttacks::AttackType::SPELL, 1);
		}
		else if (Input::GetControllerButtonDown(player->controller_index, player->controller_jump))
		{
			player->attacks->ReceiveInput(PlayerAttacks::AttackType::SPELL, 2);
		}
		else if (Input::GetControllerButtonDown(player->controller_index, player->controller_light_attack))
		{
			player->attacks->ReceiveInput(PlayerAttacks::AttackType::SPELL, 3);
		}
	}
	else if (Input::GetControllerButtonDown(player->controller_index, player->controller_light_attack)
		|| Input::GetKeyDown(player->keyboard_light_attack))
		player->attacks->ReceiveInput(PlayerAttacks::AttackType::LIGHT);
	else if (Input::GetControllerButtonDown(player->controller_index, player->controller_heavy_attack)
		|| Input::GetKeyDown(player->keyboard_heavy_attack))
		player->attacks->ReceiveInput(PlayerAttacks::AttackType::HEAVY);

	if ((Input::GetControllerButtonDown(player->controller_index, player->controller_dash)
		|| Input::GetKeyDown(player->keyboard_dash)) && player->attacks->CanBeInterrupted()) {
		return new RollingState();
	}

	if ((Input::GetControllerButtonDown(player->controller_index, player->controller_jump)
		|| Input::GetKeyDown(player->keyboard_jump)) && player->attacks->CanBeInterrupted() && player->controller->isGrounded) {
		player->Jump();
	}

	return nullptr;
}

void AttackingState::Update(PlayerController* player)
{
	player->attacks->UpdateCurrentAttack();
}

State* AttackingState::OnAnimationEnd(PlayerController* player, const char* name)
{
	return nullptr;
}

void AttackingState::OnEnter(PlayerController* player)
{
	player->audio->StartSound("Hit_Sword");
}

void AttackingState::OnExit(PlayerController* player)
{
	player->attacks->CancelAttack();
}

void RollingState::Update(PlayerController* player)
{
	player->player_data.speed += player->player_data.speed * player->player_data.slow_speed * Time::GetDT();
	player->UpdateDashEffect();
}

State* RollingState::OnAnimationEnd(PlayerController* player, const char* name)
{
	if (!player->mov_input)
	{
		return new IdleState();
	}
	else
	{
		return new RunningState();
	}

	return nullptr;
}

void RollingState::OnEnter(PlayerController* player)
{
	if (player->mov_input)
	{
		float3 direction_vector = player->GetDirectionVector();

		player->player_data.speed = direction_vector.Normalized() * player->player_data.stats["Dash_Power"].GetValue();

		float angle_dir = atan2f(direction_vector.z, direction_vector.x);
		Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle_dir * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());
		player->transform->SetGlobalRotation(rot);
	}
	else
		player->player_data.speed = player->transform->forward * player->player_data.stats["Dash_Power"].GetValue();

	player->animator->PlayState("Roll");
	player->last_dash_position = player->transform->GetGlobalPosition();
}

void RollingState::OnExit(PlayerController* player)
{

}

void HitState::Update(PlayerController* player)
{
	player->player_data.speed += player->player_data.speed * player->player_data.slow_speed * Time::GetDT();
}

State* HitState::OnAnimationEnd(PlayerController* player, const char* name)
{
	if (strcmp(name, "Hit") == 0) {
		player->animator->SetBool("reviving", false);
		player->player_data.speed = float3::zero();
		return new IdleState();
	}
	return nullptr;
}

void HitState::OnEnter(PlayerController* player)
{
	if (player->player_being_revived != nullptr)
	{
		((DeadState*)player->player_being_revived->state)->revive_world_ui->GetComponentInChildren<MiniGame_Revive>()->RestartMinigame();
	}
}

void HitState::OnExit(PlayerController* player)
{
}

void RevivingState::Update(PlayerController* player)
{

}

void RevivingState::OnEnter(PlayerController* player)
{
	player->input_blocked = true;
	player->player_data.speed = float3::zero();
	player->animator->SetBool("reviving", true);
	((DeadState*)player->player_being_revived->state)->revive_world_ui->GetComponentInChildren<MiniGame_Revive>()->StartMinigame(player);
}

void RevivingState::OnExit(PlayerController* player)
{
	player->animator->SetBool("reviving", false);
	if(player->player_being_revived->state->type != StateType::DEAD)player->player_being_revived = nullptr;
	player->input_blocked = false;
}

void DeadState::OnEnter(PlayerController* player)
{
	player->animator->PlayState("Death");
	player->animator->SetBool("dead", true);
	player->player_data.speed = float3::zero();
	player->is_immune = true;
	GameManager::instance->event_manager->OnPlayerDead(player);
	float3 vector = (Camera::GetCurrentCamera()->game_object_attached->transform->GetGlobalPosition() - player->game_object->transform->GetGlobalPosition()).Normalized();
	revive_world_ui = GameObject::Instantiate(player->revive_world_ui, float3(player->game_object->transform->GetGlobalPosition().x + vector.x, player->game_object->transform->GetGlobalPosition().y + vector.y + 1, player->game_object->transform->GetGlobalPosition().z + vector.z));
	revive_world_ui->transform->SetLocalScale(1, 1, 1);
	revive_world_ui->SetNewParent(player->game_object);
	revive_world_ui->GetComponentInChildren<MiniGame_Revive>()->player_dead = player;
}

void DeadState::OnExit(PlayerController* player)
{
}
