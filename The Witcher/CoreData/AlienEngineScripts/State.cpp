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
	State* ret = nullptr;

	if (player->movement_input.Length() > 0)
	{
		return new RunningState();
	}
	ret = GroundState::HandleInput(player);

	return ret;
}

void IdleState::Update(PlayerController* player)
{
	if (!player->is_grounded)
	{
		player->Fall();
	}
}

void IdleState::OnEnter(PlayerController* player)
{
	player->player_data.velocity = float3::zero();
}

void IdleState::OnExit(PlayerController* player)
{
}

State* RunningState::HandleInput(PlayerController* player)
{
	State* ret = nullptr;

	if (!player->mov_input)
	{
		return new IdleState();
	}
	ret = GroundState::HandleInput(player);

	return ret;
}

void RunningState::Update(PlayerController* player)
{
	if (Time::GetGameTime() - player->timer >= player->delay_footsteps) {
		player->timer = Time::GetGameTime();
		player->audio->StartSound("Player_Footstep");
	}
	player->HandleMovement();

	if (!player->is_grounded)
	{
		player->Fall();
	}

	float lerp = player->player_data.velocity.Length() / player->player_data.stats["Movement_Speed"].GetValue();
	lerp = lerp / Time::GetScaleTime();
	player->animator->SetStateSpeed("Run", lerp);
}

void RunningState::OnEnter(PlayerController* player)
{
	bool found_running = false; 

	for (auto it = player->particles.begin(); it != player->particles.end(); ++it)
	{
		if (std::strcmp((*it)->GetName(), "p_run") == 0)
		{
			found_running = true; 
			(*it)->GetComponent<ComponentParticleSystem>()->OnEmitterPlay();

			// sub-emitters 
			for(auto& child : (*it)->GetChildren())
				child->GetComponent<ComponentParticleSystem>()->OnEmitterPlay();

			break;
		}
	}

	if(found_running == false)
		player->SpawnParticle("p_run", float3(0.f, 0.f, -0.15f));	
	
	player->audio->StartSound("Player_Footstep");
	player->timer = Time::GetGameTime();
}

void RunningState::OnExit(PlayerController* player)
{
	for (auto it = player->particles.begin(); it != player->particles.end(); ++it)
	{
		if (std::strcmp((*it)->GetName(), "p_run") == 0)
		{
			(*it)->GetComponent<ComponentParticleSystem>()->OnEmitterStop();

			// sub-emitters 
			for (auto& child : (*it)->GetChildren())
				child->GetComponent<ComponentParticleSystem>()->OnEmitterStop();

			break;
		}
	}
}

State* JumpingState::HandleInput(PlayerController* player)
{
	if (player->is_grounded)
	{
		player->audio->StartSound("Player_Fall");

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
	bool found_jumping = false;

	for (auto it = player->particles.begin(); it != player->particles.end(); ++it)
	{
		if (std::strcmp((*it)->GetName(), "p_jump") == 0)
		{
			found_jumping = true;
			auto particles = (*it)->GetComponent<ComponentParticleSystem>(); 
			particles->OnStop();
			particles->GetSystem()->emmitter.ResetBursts();
			particles->OnPlay();
			break;
		}
	}

	if (found_jumping == false)
		player->SpawnParticle("p_jump");

	player->animator->SetBool("air", true);
}

void JumpingState::OnExit(PlayerController* player)
{
	bool found_jumping = false;

	for (auto it = player->particles.begin(); it != player->particles.end(); ++it)
	{
		if (std::strcmp((*it)->GetName(), "p_jump") == 0)
		{
			found_jumping = true;
			auto particles = (*it)->GetComponent<ComponentParticleSystem>();
			particles->OnStop();
			particles->GetSystem()->emmitter.ResetBursts();
			particles->OnPlay();
			break;
		}
	}

	player->animator->SetBool("air", false);
}

State* AttackingState::HandleInput(PlayerController* player)
{
	if (Input::GetControllerTriggerLeft(player->controller_index) == 1.0
		|| (Input::GetKeyDown(player->keyboard_spell_1) || Input::GetKeyDown(player->keyboard_spell_2) || Input::GetKeyDown(player->keyboard_spell_3) || Input::GetKeyDown(player->keyboard_spell_4))) {

		if (Input::GetControllerButtonDown(player->controller_index, player->controller_heavy_attack) || Input::GetKeyDown(player->keyboard_spell_1))
		{
			player->attacks->ReceiveInput(PlayerAttacks::AttackType::SPELL, 0);
		}
		else if (Input::GetControllerButtonDown(player->controller_index, player->controller_revive) || Input::GetKeyDown(player->keyboard_spell_2))
		{
			player->attacks->ReceiveInput(PlayerAttacks::AttackType::SPELL, 1);
		}
		else if (Input::GetControllerButtonDown(player->controller_index, player->controller_jump) || Input::GetKeyDown(player->keyboard_spell_3))
		{
			player->attacks->ReceiveInput(PlayerAttacks::AttackType::SPELL, 2);
		}
		else if (Input::GetControllerButtonDown(player->controller_index, player->controller_light_attack) || Input::GetKeyDown(player->keyboard_spell_4))
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
		|| Input::GetKeyDown(player->keyboard_jump)) && player->attacks->CanBeInterrupted() && player->is_grounded) {
		player->Jump();
		return new JumpingState();
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

}

void AttackingState::OnExit(PlayerController* player)
{
	player->attacks->CancelAttack();
}

void RollingState::Update(PlayerController* player)
{
	player->player_data.velocity += player->player_data.velocity * player->player_data.slow_speed * Time::GetDT();
	player->UpdateDashEffect();

	if (player->player_data.type == PlayerController::PlayerType::YENNEFER)
	{
		float current_speed = player->animator->GetCurrentStateSpeed();
		float target_speed = current_speed + player->dashData.current_acel_multi * Time::GetDT();

		if (target_speed > player->dashData.max_speed)
			target_speed = player->dashData.max_speed;
		else if (target_speed < player->dashData.min_speed)
			target_speed = player->dashData.min_speed;

		player->animator->SetStateSpeed("Roll", target_speed);
	}
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

		player->player_data.velocity = direction_vector.Normalized() * player->player_data.stats["Dash_Power"].GetValue();

		float angle_dir = atan2f(direction_vector.z, direction_vector.x);
		Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle_dir * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());
		player->transform->SetGlobalRotation(rot);
	}
	else
		player->player_data.velocity = player->transform->forward * player->player_data.stats["Dash_Power"].GetValue();

	player->animator->PlayState("Roll");
	player->audio->StartSound("Play_Roll");
	player->last_dash_position = player->transform->GetGlobalPosition();

	// Special stuff to make it look cooler
	if (player->player_data.type == PlayerController::PlayerType::YENNEFER)
	{
		// Animation
		if (player->dashData.start_speed == 0.0f)
			player->dashData.start_speed = player->animator->GetCurrentStateSpeed();

		// Particles
		bool found_dash = false;

		for (auto it = player->particles.begin(); it != player->particles.end(); ++it)
		{
			if (std::strcmp((*it)->GetName(), "Y_Dash_Particle_Emitter") == 0)
			{
				found_dash = true;
				(*it)->GetComponent<ComponentParticleSystem>()->OnEmitterPlay();

				// sub-emitters 
				for (auto& child : (*it)->GetChildren())
					child->GetComponent<ComponentParticleSystem>()->OnEmitterPlay();

				break;
			}
		}

		if (found_dash == false)
			player->SpawnParticle("Y_Dash_Particle_Emitter", float3(0.f, 0.15f, 0.f));
	}

	else if (player->player_data.type == PlayerController::PlayerType::GERALT)
	{
		player->dashData.dash_trail->Start();
	}
		
}

void RollingState::OnExit(PlayerController* player)
{
	if (player->player_data.type == PlayerController::PlayerType::YENNEFER)
	{
		// Animation
		player->ToggleDashMultiplier();

		// Particles
		for (auto it = player->particles.begin(); it != player->particles.end(); ++it)
		{
			if (std::strcmp((*it)->GetName(), "Y_Dash_Particle_Emitter") == 0)
			{
				(*it)->GetComponent<ComponentParticleSystem>()->OnEmitterStop();

				// sub-emitters 
				for (auto& child : (*it)->GetChildren())
					child->GetComponent<ComponentParticleSystem>()->OnEmitterStop();

				break;
			}
		}

	}
	else if (player->player_data.type == PlayerController::PlayerType::GERALT)
	{
		player->dashData.dash_trail->Stop();	
	}
		
}

void HitState::Update(PlayerController* player)
{
	player->player_data.velocity += player->player_data.velocity * player->player_data.slow_speed * Time::GetDT();
}

State* HitState::OnAnimationEnd(PlayerController* player, const char* name)
{
	if (strcmp(name, "Hit") == 0) {
		player->animator->SetBool("reviving", false);
		player->player_data.velocity = float3::zero();
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
	player->player_data.velocity = float3::zero();
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
	//player->play
	player->player_data.velocity = float3::zero();
	player->is_immune = true;
	GameManager::instance->event_manager->OnPlayerDead(player);
	float3 vector = (Camera::GetCurrentCamera()->game_object_attached->transform->GetGlobalPosition() - player->game_object->transform->GetGlobalPosition()).Normalized();
	revive_world_ui = GameObject::Instantiate(player->revive_world_ui, float3(player->game_object->transform->GetGlobalPosition().x + vector.x, player->game_object->transform->GetGlobalPosition().y + vector.y + 1, player->game_object->transform->GetGlobalPosition().z + vector.z));
	revive_world_ui->transform->SetLocalScale(0.1f, 0.1f, 0.1f);
	revive_world_ui->SetNewParent(player->game_object);
	revive_world_ui->GetComponentInChildren<MiniGame_Revive>()->player_dead = player;
}

void DeadState::OnExit(PlayerController* player)
{
}

State* GroundState::HandleInput(PlayerController* player)
{
	if (!player->is_grounded)
	{
		player->Fall();
		return new JumpingState();
	}

	if (Input::GetControllerTriggerLeft(player->controller_index) == 1.0
		|| (Input::GetKeyDown(player->keyboard_spell_1) || Input::GetKeyDown(player->keyboard_spell_2) || Input::GetKeyDown(player->keyboard_spell_3) || Input::GetKeyDown(player->keyboard_spell_4))) {

		if (Input::GetControllerButtonDown(player->controller_index, player->controller_heavy_attack) || Input::GetKeyDown(player->keyboard_spell_1))
		{
			if (player->attacks->StartSpell(0))
				return new AttackingState();
		}
		else if (Input::GetControllerButtonDown(player->controller_index, player->controller_revive) || Input::GetKeyDown(player->keyboard_spell_2))
		{
			if (player->attacks->StartSpell(1))
				return new AttackingState();
		}
		else if (Input::GetControllerButtonDown(player->controller_index, player->controller_jump) || Input::GetKeyDown(player->keyboard_spell_3))
		{
			if (player->attacks->StartSpell(2))
				return new AttackingState();
		}
		else if (Input::GetControllerButtonDown(player->controller_index, player->controller_light_attack) || Input::GetKeyDown(player->keyboard_spell_4))
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
		|| Input::GetKeyDown(player->keyboard_jump) && player->is_grounded)
	{
		player->Jump();
		return new JumpingState();
	}

	if (Input::GetControllerButtonDown(player->controller_index, player->controller_revive)
		|| Input::GetKeyDown(player->keyboard_revive)) {
		if (player->CheckForPossibleRevive()) {
			player->player_data.velocity = float3::zero();
			player->animator->SetBool("reviving", true);
			return new RevivingState();
		}
	}

	return nullptr;
}
