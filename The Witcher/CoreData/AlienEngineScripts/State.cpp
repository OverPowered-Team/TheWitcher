#include "GameManager.h"
#include "RumblerManager.h"
#include "EventManager.h"

#include "PlayerController.h"
#include "PlayerAttacks.h"

#include "State.h"


State* IdleState::HandleInput(PlayerController* player)
{
	if (!player->controller->isGrounded)
	{
		player->Fall();
		return new JumpingState();
	}
	if (player->movement_input.Length() > 0)
	{
		return new RunningState();
	}
	if (Input::GetControllerButtonDown(player->controller_index, player->controller_light_attack)
		|| Input::GetKeyDown(player->keyboard_light_attack)) {
		player->attacks->StartAttack(PlayerAttacks::AttackType::LIGHT);
		return new AttackingState();
	}
	else if (Input::GetControllerButtonDown(player->controller_index, player->controller_heavy_attack)
		|| Input::GetKeyDown(player->keyboard_heavy_attack)) {
		player->attacks->StartAttack(PlayerAttacks::AttackType::HEAVY);
		GameManager::instance->rumbler_manager->StartRumbler(RumblerType::HEAVY_ATTACK, player->controller_index);
		return new AttackingState();
	}

	if (Input::GetControllerButtonDown(player->controller_index, player->controller_spell)
		|| Input::GetKeyDown(player->keyboard_spell)) {
		player->attacks->StartSpell(0);
		return new CastingState();
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
	if (!player->controller->isGrounded)
	{
		player->Fall();
		return new JumpingState();
	}

	if (!player->mov_input)
	{
		return new IdleState();
	}

	if (Input::GetControllerButtonDown(player->controller_index, player->controller_light_attack)
		|| Input::GetKeyDown(player->keyboard_light_attack)) {
		player->attacks->StartAttack(PlayerAttacks::AttackType::LIGHT);
		return new AttackingState();
	}

	if (Input::GetControllerButtonDown(player->controller_index, player->controller_heavy_attack)
		|| Input::GetKeyDown(player->keyboard_heavy_attack)) {
		player->attacks->StartAttack(PlayerAttacks::AttackType::HEAVY);
		GameManager::instance->rumbler_manager->StartRumbler(RumblerType::HEAVY_ATTACK, player->controller_index);
		return new AttackingState();
	}

	if (Input::GetControllerButtonDown(player->controller_index, player->controller_spell)
		|| Input::GetKeyDown(player->keyboard_spell)) {
		player->attacks->StartSpell(0);
		return new CastingState();
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
}

void RunningState::OnEnter(PlayerController* player)
{
	player->particles["p_run"]->SetEnable(true);
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
	if (Input::GetControllerButtonDown(player->controller_index, player->controller_light_attack)
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
	player->player_data.speed += player->player_data.speed * -0.08f;
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
	float3 direction_vector = float3::zero();

	if (player->mov_input)
	{
		direction_vector = float3(player->movement_input.x, 0.f, player->movement_input.y);
		direction_vector = Camera::GetCurrentCamera()->game_object_attached->transform->GetGlobalRotation().Mul(direction_vector);
		direction_vector.y = 0.f;
	}
	else
		direction_vector = player->transform->forward;

	player->player_data.speed = direction_vector * player->player_data.stats["Dash_Power"].GetValue();
	player->animator->PlayState("Roll");

	player->is_immune = true;
}

void RollingState::OnExit(PlayerController* player)
{
	player->is_immune = false;
}

void CastingState::Update(PlayerController* player)
{
	player->attacks->UpdateCurrentAttack();
}

State* CastingState::OnAnimationEnd(PlayerController* player, const char* name)
{
	return nullptr;
}

void CastingState::OnEnter(PlayerController* player)
{
}

void CastingState::OnExit(PlayerController* player)
{
}

void HitState::Update(PlayerController* player)
{
	player->player_data.speed += player->player_data.speed * -0.08f;
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
}

void HitState::OnExit(PlayerController* player)
{
}

State* RevivingState::OnAnimationEnd(PlayerController* player, const char* name)
{
	if (strcmp(name, "RCP") == 0) {
		player->ActionRevive();
		return new IdleState();
	}
	return nullptr;
}

void RevivingState::OnEnter(PlayerController* player)
{
	player->player_data.speed = float3::zero();
	player->animator->SetBool("reviving", true);
}

void RevivingState::OnExit(PlayerController* player)
{
}

void DeadState::OnEnter(PlayerController* player)
{
	player->animator->PlayState("Death");
	player->animator->SetBool("dead", true);
	player->player_data.speed = float3::zero();
	GameManager::instance->event_manager->OnPlayerDead(player);
}

void DeadState::OnExit(PlayerController* player)
{
}
