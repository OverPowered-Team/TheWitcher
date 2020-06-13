#include "GameManager.h"
#include "PlayerManager.h"
#include "ParticlePool.h"
#include "PlayerAttacks.h"
#include "EventManager.h"
#include "EffectsFactory.h"
#include "RelicBehaviour.h"
#include "Effect.h"
#include "CameraMovement.h"
#include "Enemy.h"
#include "GameManager.h"
#include "RumblerManager.h"
#include "State.h"
#include "../../ComponentDeformableMesh.h"
#include "CameraShake.h"
#include "../../ComponentTrail.h"
#include "Bonfire.h"
#include "Scores_Data.h"
#include "UI_DamageCount.h"
#include "InGame_UI.h"
#include "DashCollider.h"

#include "UI_Char_Frame.h"
#include "PlayerController.h"

PlayerController::PlayerController() : Alien()
{
}

PlayerController::~PlayerController()
{
}

void PlayerController::Start()
{
	animator = GetComponent<ComponentAnimator>();
	controller = GetComponent<ComponentCharacterController>();
	attacks = GetComponent<PlayerAttacks>();
	audio = GetComponent<ComponentAudioEmitter>();
	camera = Camera::GetCurrentCamera();
	shake = camera->game_object_attached->GetComponent<CameraShake>();
	particle_spawn_positions = game_object->GetChild("Particle_Positions")->GetChildren();
	
	LoadStats();
	CalculateAABB();
	InitKeyboardControls();

	// TP to last checkpoint if checkpoint exist
	if (Scores_Data::last_checkpoint_position.IsFinite())
	{
		this->controller->SetPosition(float3(
			Scores_Data::last_checkpoint_position.x + (controller_index - 1),
			Scores_Data::last_checkpoint_position.y, 
			Scores_Data::last_checkpoint_position.z)
		);
	}

	state = new IdleState();
	state->OnEnter(this);

	layers.push_back("Ground");
	layers.push_back("Default");
	layers.push_back("Player");

	// Dash
	//dashData.current_acel_multi = dashData.accel_multi; 
	GameObject* trail_go = game_object->GetChild("trail");
	if(trail_go)
		dash_trail = trail_go->GetComponent<ComponentTrail>();
	
	if (dash_trail != nullptr) //todo no ser tant guarro
	{
		dash_trail->Stop();
	}
}

void PlayerController::Update()
{
	if (Time::IsGamePaused())
		return;

	UpdateInput();

	//State Machine--------------------------------------------------------
	State* new_state = state->HandleInput(this);
	if (new_state != nullptr)
		SwapState(new_state);

	state->Update(this);
	//---------------------------------------------------------------------

	//Effects-----------------------------
	EffectsUpdate();

	//Movement
	player_data.vertical_speed -= player_data.gravity * Time::GetDT();
	player_data.velocity.y += player_data.vertical_speed;

	if (CheckBoundaries() && can_move)
	{
		controller->Move(player_data.velocity * Time::GetDT() / Time::GetScaleTime());
	}

	if (is_grounded)
	{
		player_data.vertical_speed = 0;
	}

	//Update animator variables
	animator->SetFloat("speed", float3(player_data.velocity.x, 0, player_data.velocity.z).Length());
	animator->SetBool("movement_input", mov_input);

	//Battle circle
	CheckEnemyCircle();

	CheckGround();

	if (layer_changed && collision_timer < Time::GetTimeSinceStart())
	{
		controller->SetCollisionLayer("Player");
		layer_changed = false;
	}
}

void PlayerController::CheckGround()
{
	direction = GetDirectionVector();

	RaycastHit hit;
	is_grounded = false;
	float ground_distance = 0.3F;
	
	//raycast
	float3 center_position = transform->GetGlobalPosition();
	center_position.y += 0.5f;

	Physics::SetQueryHitTriggers(false);
	Physics::SetQueryInitialOverlaping(false);

	//if (Physics::Raycast(center_position, -float3::unitY(), 10.f, hit, Physics::GetLayerMask(layers)))
	//if (Physics::CapsuleCast(cast_transform, 0.1, 0.25, -float3::unitY(), 10.f, hit, Physics::GetLayerMask(layers)))
	if(Physics::SphereCast(center_position, 0.3f, -float3::unitY(), 2.0f, hit, Physics::GetLayerMask(layers)))
	{
		if (transform->GetGlobalPosition().Distance(hit.point) < ground_distance)
		{
			float angle = RadToDeg(transform->up.AngleBetween(hit.normal));
			if (angle >= 45)
				return;

			is_grounded = player_data.vertical_speed > 0 ? false : true;
			if (Physics::Raycast(center_position, -float3::unitY(), 2.f, hit, Physics::GetLayerMask(layers)))
			{
				if (transform->GetGlobalPosition().Distance(hit.point) < ground_distance)
				{
					Quat ground_rot = Quat::RotateFromTo(transform->up, hit.normal);
					direction = ground_rot * direction; //We rotate the direction vector for the amount of slope we currently are on.

					/*float angle = RadToDeg(transform->up.AngleBetween(hit.normal));
					if (angle > 45 && direction.y > 0)
						is_grounded = false;*/
				}
			}
		}
	}
}

void PlayerController::ChangeCollisionLayer(std::string layer, float time)
{
	controller->SetCollisionLayer(layer);
	collision_timer = Time::GetTimeSinceStart() + time;
	layer_changed = true;
}

void PlayerController::UpdateInput()
{
	float2 joystickInput = float2(Input::GetControllerHoritzontalLeftAxis(controller_index), Input::GetControllerVerticalLeftAxis(controller_index));
	float2 keyboardInput = float2::zero();

	if (Input::GetKeyRepeat(keyboard_move_left)) {
		keyboardInput.x = 1.f;
	}
	if (Input::GetKeyRepeat(keyboard_move_right)) {
		keyboardInput.x = -1.f;
	}
	if (Input::GetKeyRepeat(keyboard_move_down)) {
		keyboardInput.y = -1.f;
	}
	if (Input::GetKeyRepeat(keyboard_move_up)) {
		keyboardInput.y = 1.f;
	}

	if (keyboardInput.Length() > 0 && !input_blocked)
	{
		mov_input = true;
		keyboardInput.Normalize();
		movement_input = keyboardInput;
	}
	else if (joystickInput.Length() > 0 && !input_blocked) {
		mov_input = true;
		movement_input = joystickInput;
	}
	else
	{
		mov_input = false;
		movement_input = float2::zero();
	}


	// DEBUG
	if (Input::GetKeyDown(SDL_SCANCODE_KP_9) && (player_data.type == PlayerController::PlayerType::YENNEFER))
		ReceiveDamage(10, float3::zero(), false); 
}

void PlayerController::SetState(StateType new_state)
{
	state->OnExit(this);
	delete state;

	switch (new_state)
	{
	case StateType::IDLE:
		state = new IdleState();
		break;
	case StateType::RUNNING:
		state = new RunningState();
		break;
	case StateType::ATTACKING:
		state = new AttackingState();
		break;
	case StateType::JUMPING:
		state = new JumpingState();
		break;
	case StateType::ROLLING:
		state = new RollingState();
		break;
	case StateType::DEAD:
		state = new DeadState();
		break;
	case StateType::REVIVING:
		state = new RevivingState();
		break;
	case StateType::HIT:
		state = new HitState();
		break;
	default:
		state = new IdleState();
		break;
	}

	state->OnEnter(this);
}

void PlayerController::SwapState(State* new_state)
{
	state->OnExit(this);
	delete state;
	state = new_state;
	state->OnEnter(this);
}

void PlayerController::ApplyRoot(float time)
{
	can_move = false;
	Invoke(std::bind(&PlayerController::ReleaseRoot, this), time);
}

void PlayerController::ReleaseRoot()
{
	can_move = true;
}

bool PlayerController::AnyKeyboardInput()
{
	return Input::GetKeyDown(keyboard_move_up)
		|| Input::GetKeyDown(keyboard_move_down)
		|| Input::GetKeyDown(keyboard_move_left)
		|| Input::GetKeyDown(keyboard_move_right)
		|| Input::GetKeyDown(keyboard_dash)
		|| Input::GetKeyDown(keyboard_jump);
}

void PlayerController::HandleMovement()
{	
	player_data.velocity = direction * player_data.stats["Movement_Speed"].GetValue() * movement_input.Length();

	//rotate
	if (mov_input)
	{
		transform->SetGlobalRotation(Quat::RotateAxisAngle(float3::unitY(), atan2f(direction.x, direction.z)));
	}
}

void PlayerController::OnDrawGizmos()
{
	/*float4x4 matrix = transform->GetGlobalMatrix();
	matrix = matrix.Translate(float3(0, 0.5f, 0)); //middle point of character
	float3 origin = matrix.TranslatePart();
	RaycastHit hit;

	if (Physics::CapsuleCast(matrix, 0.1f, 0.25f, -float3::unitY(), 0.5f, hit, Physics::GetLayerMask("Ground")))
	{
		Gizmos::DrawLine(origin, hit.point, Color::Green());
		float rest_dist = 0.2f - origin.Distance(hit.point);
		Gizmos::DrawLine(hit.point, hit.point + hit.normal * rest_dist, Color::Green());
	}*/
}

void PlayerController::EffectsUpdate()
{
	if (Time::GetGameTime() - last_regen_tick > 1.0f)
	{
		player_data.stats["Chaos"].IncreaseStat(player_data.stats["Chaos_Regen"].GetValue());
		//player_data.stats["Health"].IncreaseStat(player_data.stats["Health_Regen"].GetValue());

		if (HUD)
		{
			//HUD->GetComponent<UI_Char_Frame>()->LifeChange(player_data.stats["Health"].GetValue(), player_data.stats["Health"].GetMaxValue());
			HUD->GetComponent<UI_Char_Frame>()->ManaChange(player_data.stats["Chaos"].GetValue(), player_data.stats["Chaos"].GetMaxValue());
		}

		last_regen_tick = Time::GetGameTime();
	}

	for (auto it = effects.begin(); it != effects.end();)
	{
		if ((*it)->UpdateEffect() && (*it)->ticks_time > 0)
		{
			for (auto it_stats = player_data.stats.begin(); it_stats != player_data.stats.end(); ++it_stats)
			{
				it_stats->second.ModifyCurrentStat((*it));

				//Temporal solution
				if (it_stats->first == "Health")
				{
					HUD->GetComponent<UI_Char_Frame>()->LifeChange(player_data.stats["Health"].GetValue(), player_data.stats["Health"].GetMaxValue());
					if (player_data.stats["Health"].GetValue() <= 0)
					{
						shake->Shake(0.16f, 1, 5.f, 0.5f, 0.5f, 0.5f);
						Die();
					}
				}
			}
			if ((*it)->spawned_particle != nullptr)
			{
				(*it)->spawned_particle->SetEnable(false);
				(*it)->spawned_particle->SetEnable(true);
			}

		}

		if ((*it)->to_delete)
		{
			it = RemoveEffect(it);
			continue;
		}
		++it;
	}
}

void PlayerController::PlayAttackParticle()
{
	if (attacks->GetCurrentAttack())
	{
		SpawnParticle(attacks->GetCurrentAttack()->info.particle_name, attacks->GetCurrentAttack()->info.particle_pos);
		
		if(attacks->GetCurrentAttack()->IsLast())
			ChangeColorParticle();

		/*particles[attacks->GetCurrentAttack()->info.particle_name]->SetEnable(false);
		particles[attacks->GetCurrentAttack()->info.particle_name]->SetEnable(true);*/
	}
}

void PlayerController::PlayAllowParticle()
{
	if (attacks->GetCurrentAttack())
	{
		//SpawnParticle(attacks->GetCurrentAttack()->info.allow_combo_p_name);
		/*particles.insert(std::pair(attacks->GetCurrentAttack()->info.allow_combo_p_name,
			GameManager::instance->particle_pool->GetInstance(attacks->GetCurrentAttack()->info.allow_combo_p_name,
				transform->GetGlobalPosition(), this->game_object)));*/
		/*particles[attacks->GetCurrentAttack()->info.allow_combo_p_name]->SetEnable(false);
		particles[attacks->GetCurrentAttack()->info.allow_combo_p_name]->SetEnable(true);*/
	}
}

void PlayerController::ReleaseAttackParticle()
{
	ReleaseParticle(attacks->GetCurrentAttack()->info.particle_name);
	ReleaseParticle(attacks->GetCurrentAttack()->info.allow_combo_p_name);
	ReleaseParticle(attacks->GetCurrentAttack()->info.chain_particle);
}

#pragma region PlayerActions
void PlayerController::Jump()
{
	player_data.vertical_speed = player_data.stats["Jump_Power"].GetValue();
	is_grounded = false;
	animator->PlayState("Air");
	switch (player_data.type)
	{
	case PlayerController::PlayerType::GERALT:
		audio->StartSound("Play_GeraltJump");
		break;
	case PlayerController::PlayerType::YENNEFER:
		audio->StartSound("Play_YennJump");
		break;
	}
	animator->SetBool("air", true);
}

void PlayerController::Fall()
{
	animator->PlayState("Air");
	animator->SetBool("air", true);
}

void PlayerController::Die()
{
	SetState(StateType::DEAD);
}
void PlayerController::PickUpRelic(Relic* _relic)
{
	relics.push_back(_relic);

	for (int i = 0; i < _relic->effects.size(); i++)
	{
		AddEffect(_relic->effects.at(i));
	}
}
void PlayerController::Revive(float minigame_value)
{
	animator->SetBool("dead", false);
	animator->PlayState("Revive");
	player_data.stats["Health"].IncreaseStat(player_data.stats["Health"].GetMaxValue() * minigame_value);

	if(HUD)
		HUD->GetComponent<UI_Char_Frame>()->LifeChange(player_data.stats["Health"].GetValue(), player_data.stats["Health"].GetMaxValue());

	if(GameManager::instance->rumbler_manager)
		GameManager::instance->rumbler_manager->StartRumbler(RumblerType::REVIVE, controller_index);
	if(GameManager::instance->event_manager)
		GameManager::instance->event_manager->OnPlayerRevive(this);
}

void PlayerController::ReceiveDamage(float dmg, float3 knock_speed, bool knock)
{
	if (!godmode && !is_immune)
	{
		if (player_data.stats["Absorb"].GetValue() > 0)
		{
			AbsorbHit();
			return;
		}

		if (player_data.stats["Health"].GetValue() <= 0.0f)
			return;

		player_data.stats["Health"].DecreaseStat(dmg);

		switch (player_data.type)
		{
		case PlayerController::PlayerType::GERALT:
			audio->StartSound("Play_GeraltGetDamaged");
			break;
		case PlayerController::PlayerType::YENNEFER:
			audio->StartSound("Play_YennGetDamaged");
			break;
		}

		if (HUD)
		{
			HUD->GetComponent<UI_Char_Frame>()->LifeChange(player_data.stats["Health"].GetValue(), player_data.stats["Health"].GetMaxValue());
		}

		attacks->CancelAttack();
		if (player_data.stats["Health"].GetValue() == 0)
		{
			shake->Shake(0.16f, 1, 5.f, 0.5f, 0.5f, 0.5f);
			Die();
		}
		else if(knock)
		{
			animator->PlayState("Hit");
			player_data.velocity = knock_speed;
			player_data.vertical_speed = knock_speed.y;
			SetState(StateType::HIT);
		}

		if (GameManager::instance->rumbler_manager)
			GameManager::instance->rumbler_manager->StartRumbler(RumblerType::RECEIVE_HIT, controller_index);

		// Heartbeat effect 
		static float percentage = 0.f, thresholdPercentage = 0.3f;
		static bool playing = false;

		percentage = player_data.stats["Health"].GetValue() / player_data.stats["Health"].GetMaxValue();
		if (playing == false)
		{
			if (percentage <= thresholdPercentage)
			{
				playing = true;
				audio->StartSound("Play_Heartbeats");
			}
		}
		else
		{
			if (percentage > thresholdPercentage)
			{
				playing = false;
				audio->StopSoundByName("Play_Heartbeats");
			}
		}

		audio->SetRTPCValue("PlayerLife", player_data.stats["Health"].GetValue());
	}
}

void PlayerController::AbsorbHit()
{
	for (auto it = effects.begin(); it != effects.end(); ++it)
	{
		for (auto mods = (*it)->additive_modifiers.begin(); mods != (*it)->additive_modifiers.end(); ++mods)
		{
			if (mods->identifier == "Absorb")
			{
				mods->amount--;
				if (mods->amount <= 0)
				{
					RemoveEffect(it);
				}
				return;
			}
		}
	}
}
#pragma endregion PlayerActions

void PlayerController::HitByRock(float time, float dmg)
{
	if (state->type != StateType::DEAD || !is_immune) {
		ReceiveDamage(dmg, float3::zero(), false);
		Invoke(std::bind(&PlayerController::RecoverFromRockHit, this), time);
		transform->SetLocalScale(1.f, 0.25f, 1.f);
		if (state->type != StateType::DEAD) {
			is_immune = true;
		}
	}
}

void PlayerController::RecoverFromRockHit()
{
	is_immune = false;
	transform->SetLocalScale(1.f, 1.f, 1.f);
}

void PlayerController::AddEffect(Effect* _effect)
{
	for (auto it = effects.begin(); it != effects.end(); ++it)
	{
		if ((*it)->name == _effect->name)
		{
			(*it)->start_time = Time::GetGameTime(); //Refresh timer
			delete _effect;
			return;
		}
	}

	effects.push_back(_effect);

	if (std::strcmp(_effect->vfx_on_apply.c_str(), "") != 0)
		_effect->spawned_particle = GameManager::instance->particle_pool->GetInstance(_effect->vfx_on_apply,
			particle_spawn_positions[_effect->vfx_position]->transform->GetLocalPosition(), float3::zero(), this->game_object, true);

	if (dynamic_cast<AttackEffect*>(_effect) != nullptr)
	{
		attacks->OnAddAttackEffect(((AttackEffect*)_effect));
	}
	else
	{
		for (auto it = player_data.stats.begin(); it != player_data.stats.end(); ++it)
		{
			if(_effect->AffectsStat(it->second.name) && _effect->ticks_time <= 0)
				it->second.ApplyEffect(_effect);
		}
	}



	//GameObject* go = GameObject::Instantiate(_effect->vfx_on_apply.c_str(), {0, 0.5f, 0}, false, game_object);
	//particles.insert(std::pair(_effect->vfx_on_apply, GameManager::instance->particle_pool->GetInstance(_effect->vfx_on_apply)));
}
std::vector<Effect*>::iterator PlayerController::RemoveEffect(std::vector<Effect*>::iterator it)
{
	Effect* tmp_effect = (*it);
	if (!tmp_effect)
	{
		LOG("Error trying to remove null effect.");
		return it;
	}

	if (tmp_effect->spawned_particle != nullptr)
	{
		GameManager::instance->particle_pool->ReleaseInstance(tmp_effect->vfx_on_apply, tmp_effect->spawned_particle);
	}

	if (dynamic_cast<AttackEffect*>(tmp_effect) != nullptr)
	{
		attacks->OnRemoveAttackEffect(((AttackEffect*)tmp_effect));
	}
	else
	{
		for (auto it = player_data.stats.begin(); it != player_data.stats.end(); ++it)
		{
			if (tmp_effect->AffectsStat(it->second.name))
				it->second.RemoveEffect(tmp_effect);
		}
	}

	it = effects.erase(it);

	/*for (auto it = particles.begin(); it != particles.end();)
	{
		if (it->first == tmp_effect->vfx_on_apply)
		{
			GameManager::instance->particle_pool->ReleaseInstance(tmp_effect->vfx_on_apply, it->second);
			it = particles.erase(it);
		}
		else
			++it;
	}*/

	delete tmp_effect;
	return it;
}
bool PlayerController::CheckBoundaries()
{
	float3 next_pos = transform->GetGlobalPosition() + player_data.velocity * Time::GetDT();

	float3 moved = (next_pos - transform->GetGlobalPosition());
	AABB fake_aabb(max_aabb.minPoint + moved + transform->GetGlobalPosition(), max_aabb.maxPoint + moved + transform->GetGlobalPosition());

	float3 next_cam_pos = moved.Normalized() * 0.5 + camera->game_object_attached->transform->GetGlobalPosition(); // * 0.5 for middle point in camera

	Frustum fake_frustum = camera->frustum;
	fake_frustum.pos = next_cam_pos;

	bool contained = false;
	if (camera->frustum.Contains(fake_aabb)) {
		contained = true;
	}

	CameraMovement* cam = (CameraMovement*)camera->game_object_attached->GetComponent<CameraMovement>();
	for (int i = 0; i < cam->players.size(); ++i)
	{
		if (cam->players[i] != this->game_object)
		{
			PlayerController* const p = cam->players[i]->GetComponent<PlayerController>();
			if (p != nullptr) {
				AABB p_tmp = p->max_aabb;
				p_tmp.minPoint += cam->players[i]->transform->GetGlobalPosition();
				p_tmp.maxPoint += cam->players[i]->transform->GetGlobalPosition();

				if(camera->frustum.Contains(p_tmp) && !fake_frustum.Contains(p_tmp))
				{
					LOG("LEAVING BUDDY BEHIND");
					if (contained) {
						if (cam->state == CameraMovement::CameraState::FREE 
							|| cam->state == CameraMovement::CameraState::MOVING_TO_STATIC
							|| cam->state == CameraMovement::CameraState::MOVING_TO_AXIS
							|| cam->state == CameraMovement::CameraState::MOVING_TO_DYNAMIC
							|| cam->state == CameraMovement::CameraState::AXIS
							|| p->state->type == StateType::JUMPING
							|| cam->state == CameraMovement::CameraState::CINEMATIC)
							return true;

						cam->prev_state = cam->state;
						cam->state = CameraMovement::CameraState::FREE;
						if (cam->players[0]->transform->GetGlobalPosition().Distance(cam->transform->GetGlobalPosition()) > cam->players[1]->transform->GetGlobalPosition().Distance(cam->transform->GetGlobalPosition())) {
							cam->closest_player = 1;
						}
						else
							cam->closest_player = 0;

						cam->prev_middle = cam->transform->GetGlobalPosition() - cam->players[cam->closest_player]->transform->GetGlobalPosition();
						return true;
					}
					else {
						player_data.velocity = float3::zero();
						return false;
					}
				}
			}
		}
	}

	if (contained) {
		return true;
	}
	else {
		if (transform->GetGlobalPosition().Distance(fake_frustum.CenterPoint()) < next_pos.Distance(fake_frustum.CenterPoint())) {
			player_data.velocity = float3::zero();
			return false;
		}
	}
	return true;
}

bool PlayerController::CheckForPossibleRevive()
{
	for (int i = 0; i < GameManager::instance->player_manager->players_dead.size(); ++i) {
		float distance = this->transform->GetGlobalPosition().Distance(GameManager::instance->player_manager->players_dead[i]->transform->GetGlobalPosition());
		if (distance <= player_data.revive_range) {
			player_being_revived = GameManager::instance->player_manager->players_dead[i];
			return true;
		}
	}

	return false;
}

void PlayerController::HitFreeze(float freeze_time)
{
	if (is_immune)
		return;

	float speed = animator->GetCurrentStateSpeed();
	std::string state_name = animator->GetCurrentStateName();

	animator->SetStateSpeed(state_name.c_str(), 0);
	PauseParticle();
	is_immune = true;
	Invoke([this, speed, state_name]() -> void {this->RemoveFreeze(speed, state_name); }, freeze_time);
}

void PlayerController::RemoveFreeze(float speed, std::string state_name)
{
	ResumeParticle();
	animator->SetStateSpeed(state_name.c_str(), speed);
	
	is_immune = false;
}

void PlayerController::PauseParticle()
{
	if (!attacks->GetCurrentAttack())
		return;

	for (auto it = particles.begin(); it != particles.end(); ++it)
	{
		if (std::strcmp((*it)->GetName(), attacks->GetCurrentAttack()->info.particle_name.c_str()) == 0)
		{
			ComponentParticleSystem* p_system = (*it)->GetComponent<ComponentParticleSystem>();
			if (p_system)
				p_system->Pause();

			vector<ComponentParticleSystem*> son_particle = (*it)->GetComponentsInChildren<ComponentParticleSystem>();
			
			for (vector<ComponentParticleSystem*>::iterator ip = son_particle.begin(); ip != son_particle.end(); ++ip)
				(*ip)->Pause();
			

			return;
		}
	}

}

void PlayerController::ResumeParticle()
{
	if (!attacks->GetCurrentAttack())
		return;

	for (auto it = particles.begin(); it != particles.end(); ++it)
	{
		if (std::strcmp((*it)->GetName(), attacks->GetCurrentAttack()->info.particle_name.c_str()) == 0)
		{
			ComponentParticleSystem* p_system = (*it)->GetComponent<ComponentParticleSystem>();
			if (p_system)
				p_system->Play();

			vector<ComponentParticleSystem*> son_particle = (*it)->GetComponentsInChildren<ComponentParticleSystem>();

			for (vector<ComponentParticleSystem*>::iterator ip = son_particle.begin(); ip != son_particle.end(); ++ip)
				(*ip)->Play();

			return;
		}
	}
}

void PlayerController::SpawnParticle(std::string particle_name, float3 pos, bool local, float3 rotation, GameObject* parent)
{
	if (particle_name == "")
		return;

	for (auto it = particles.begin(); it != particles.end(); ++it)
	{
		if (std::strcmp((*it)->GetName(), particle_name.c_str()) == 0)
		{
			(*it)->SetEnable(false);

			parent = parent != nullptr ? parent : this->game_object;
			(*it)->SetNewParent(parent);
			if (local)
				(*it)->transform->SetLocalPosition(pos);
			else
				(*it)->transform->SetGlobalPosition(pos);

			(*it)->SetEnable(true);
			
			return;
		}
	}
	parent = parent != nullptr ? parent : this->game_object;
	rotation = rotation.IsZero() ? parent->transform->GetGlobalRotation().ToEulerXYZ() : rotation;
	GameObject* new_particle = GameManager::instance->particle_pool->GetInstance(particle_name, pos, rotation, parent, local);
	
	if (new_particle == nullptr)
		return;

	particles.push_back(new_particle);
	/*if (particles[particle_name])
	{
		particles[particle_name]->SetEnable(false);
		particles[particle_name]->SetEnable(true);
	}
	else
	{
		GameObject* new_particle = GameManager::instance->particle_pool->GetInstance(particle_name, pos, parent != nullptr? parent:this->game_object, local);
		particles.insert(std::pair(particle_name, new_particle));
	}*/
}

void PlayerController::SpawnDashParticle()
{
	if (player_data.type == PlayerType::YENNEFER)
	{
		float3 pos = particle_spawn_positions[1]->transform->GetGlobalPosition();
		if(!is_immune)
			pos += transform->forward * 0.8f;

		SpawnParticle("Yenn_Portal", pos, false, float3::zero(), GameManager::instance->game_object);
	}
		
}

void PlayerController::ReleaseParticle(std::string particle_name)
{
	if (particle_name == "")
		return;

	for (auto it = particles.begin(); it != particles.end();)
	{
		if (std::strcmp((*it)->GetName(), particle_name.c_str()) == 0)
		{
			GameManager::instance->particle_pool->ReleaseInstance(particle_name, (*it));
			particles.erase(it);
		}
		else
			++it;
	}

	/*if (particles[particle_name])
	{
		GameManager::instance->particle_pool->ReleaseInstance(particle_name, particles[particle_name]);
		particles.erase(particle_name);
	}*/
}
void PlayerController::ChangeColorParticle()
{
	for (auto it = particles.begin(); it != particles.end(); ++it)
	{
		if (std::strcmp((*it)->GetName(), attacks->GetCurrentAttack()->info.particle_name.c_str()) == 0)
		{
			GameObject* p_fire = (*it)->GetChild("effect_attack_fire");
			GameObject* p_ice = (*it)->GetChild("effect_attack_ice");
			GameObject* p_earth = (*it)->GetChild("effect_attack_earth");
			GameObject* p_lightning = (*it)->GetChild("effect_attack_lightning");
			GameObject* p_poison = (*it)->GetChild("effect_attack_poison");
			if (p_fire)
				p_fire->SetEnable(false);
			if (p_ice)
				p_ice->SetEnable(false);
			if (p_earth)
				p_earth->SetEnable(false);
			if (p_lightning)
				p_lightning->SetEnable(false);
			if (p_poison)
				p_poison->SetEnable(false);

			if (attacks->GetCurrentAttack()->HasTag(Attack_Tags::T_Fire))
				if (p_fire)
					p_fire->SetEnable(true);
			
			if (attacks->GetCurrentAttack()->HasTag(Attack_Tags::T_Ice))
				if (p_ice)
					p_ice->SetEnable(true);
			
			if (attacks->GetCurrentAttack()->HasTag(Attack_Tags::T_Earth))
				if (p_earth)
					p_earth->SetEnable(true);
			
			if (attacks->GetCurrentAttack()->HasTag(Attack_Tags::T_Lightning))
				if (p_lightning)
					p_lightning->SetEnable(true);
			
			if (attacks->GetCurrentAttack()->HasTag(Attack_Tags::T_Poison))
				if (p_poison)
					p_poison->SetEnable(true);
			
		}
	}
	

}
void PlayerController::CheckEnemyCircle()
{
	std::vector<ComponentCollider*> colliders = Physics::OverlapSphere(transform->GetGlobalPosition(), battleCircle);

	for (int i = 0; i < colliders.size(); ++i)
	{
		if (strcmp(colliders[i]->game_object_attached->GetTag(), "Enemy") == 0)
		{
			float3 avoid_direction = colliders[i]->game_object_attached->transform->GetGlobalPosition() - transform->GetGlobalPosition();
			float avoid_distance = avoid_direction.Length();
			if (avoid_distance > battleCircle)
				continue;

			Enemy* enemy = colliders[i]->game_object_attached->GetComponent<Enemy>();

			//LOG("Current %s attacking enemies: %i", game_object->GetName(), current_attacking_enemies);

			if (!enemy->is_battle_circle && (enemy->type == EnemyType::NILFGAARD_SOLDIER || enemy->type == EnemyType::GHOUL) && !enemy->IsRangeEnemy())
				enemy->AddBattleCircle(this);
		}
	}
}

void PlayerController::OnTerrainEnter(float4 initial_color, float4 final_color)
{
	for (auto& p : particles)
		if (strcmp(p->GetName(), "p_run") == 0 || strcmp(p->GetName(), "p_jump") == 0)
		{
			p->GetComponent<ComponentParticleSystem>()->GetSystem()->SetParticleInitialColor(initial_color);
			p->GetComponent<ComponentParticleSystem>()->GetSystem()->SetParticleFinalColor(final_color);

			// Sub-emitters
			for (auto& child : p->GetChildren())
			{
				child->GetComponent<ComponentParticleSystem>()->GetSystem()->SetParticleInitialColor(initial_color);
				child->GetComponent<ComponentParticleSystem>()->GetSystem()->SetParticleFinalColor(final_color);

			}
		}
}

void PlayerController::IncreaseStat(std::string stat, float value)
{
	auto stat_it = player_data.stats.find(stat);
	if (stat_it != player_data.stats.end())
	{
		player_data.stats[stat].IncreaseStat(value);
		if (stat == "Health" && HUD)
		{
			HUD->GetComponent<UI_Char_Frame>()->LifeChange(player_data.stats["Health"].GetValue(), player_data.stats["Health"].GetMaxValue());
		}
	}
}

#pragma region Events
void PlayerController::OnAnimationEnd(const char* name) {

	State* new_state = state->OnAnimationEnd(this, name);

	//SWAP STATE IF NEEDED
	if (new_state != nullptr)
		SwapState(new_state);
}

void PlayerController::OnHit(Enemy* enemy, float dmg_dealt)
{
	player_data.total_damage_dealt += dmg_dealt;
	HitFreeze(attacks->GetCurrentAttack()->info.freeze_time);
	attacks->OnHit(enemy);

	//EFFECT ONHIT
	for (auto it = effects.begin(); it != effects.end(); ++it)
	{
		if (dynamic_cast<AttackEffect*>(*it) != nullptr)
		{
			AttackEffect* a_effect = (AttackEffect*)(*it);
			if (a_effect->GetAttackIdentifier() == attacks->GetCurrentAttack()->info.name)
			{
				a_effect->OnHit(enemy, attacks->GetCurrentAttack()->info.name.size(), a_effect->on_hit_effect);
			}
		}
	}
}

void PlayerController::UpdateDashEffect()
{
	if (this->transform->GetGlobalPosition().DistanceSq(last_dash_position) >= 0.25)
	{
		last_dash_position = this->transform->GetGlobalPosition();
		for (auto it = effects.begin(); it != effects.end(); ++it)
		{
			if (dynamic_cast<DashEffect*>(*it) != nullptr)
			{
				GameObject* go = GameObject::Instantiate(dash_collider, this->transform->GetGlobalPosition());
				go->transform->SetGlobalRotation(this->transform->GetGlobalRotation());
				DashCollider* dash_coll = go->GetComponent<DashCollider>();
				dash_coll->effect = (DashEffect*)(*it);
				dash_coll->player_dashing = this;

				if (dash_coll->effect->on_dash_effect->name != "")
					GameManager::instance->particle_pool->GetInstance("p_" + dash_coll->effect->on_dash_effect->name, 
						float3::zero(), float3::zero(), dash_coll->game_object, true);
			}
		}
	}
}

void PlayerController::OnTriggerEnter(ComponentCollider* col)
{
	if ((strcmp("Bonfire", col->game_object_attached->GetName()) == 0))
	{
		Bonfire* bonfire = col->game_object_attached->GetComponent<Bonfire>();

		if (!bonfire->has_been_used)
		{
			if (!Scores_Data::last_checkpoint_position.Equals(bonfire->checkpoint->transform->GetGlobalPosition()))
			{
				Scores_Data::last_checkpoint_position = bonfire->checkpoint->transform->GetGlobalPosition();
				HUD->parent->parent->GetComponent<InGame_UI>()->ShowCheckpointSaved();
			}

			auto player = GameManager::instance->player_manager->players.begin();
			for (; player != GameManager::instance->player_manager->players.end(); ++player)
			{
				if (!GameManager::instance->player_manager->players_dead.empty() && GameManager::instance->player_manager->players_dead[0] == (*player))
				{
					(*player)->Revive(1);
					GameObject::Destroy((*player)->game_object->GetChild("ReviveMinigame"));
					(*player)->player_data.stats["Chaos"].IncreaseStat(player_data.stats["Chaos"].GetMaxValue());
					(*player)->HUD->GetComponent<UI_Char_Frame>()->ManaChange(player_data.stats["Chaos"].GetValue(), player_data.stats["Chaos"].GetMaxValue());
					continue;
				}

				// Heal
				(*player)->player_data.stats["Health"].IncreaseStat((*player)->player_data.stats["Health"].GetMaxValue());
				(*player)->player_data.stats["Chaos"].IncreaseStat((*player)->player_data.stats["Chaos"].GetMaxValue());
				(*player)->HUD->GetComponent<UI_Char_Frame>()->LifeChange((*player)->player_data.stats["Health"].GetValue(), (*player)->player_data.stats["Health"].GetMaxValue());
				(*player)->HUD->GetComponent<UI_Char_Frame>()->ManaChange((*player)->player_data.stats["Chaos"].GetValue(), (*player)->player_data.stats["Chaos"].GetMaxValue());
			}

			GameManager::instance->PrepareDataNextScene(false);

			// Player Used this Bonfire
			bonfire->SetBonfireUsed();
		}
	}

	if (!godmode && !is_immune)
	{
		if (strcmp(col->game_object_attached->GetTag(), "EnemyAttack") == 0) {
			Enemy* enemy = col->game_object_attached->GetComponentInParent<Enemy>();
			if (enemy) {
				//Calculate Knockback
				float3 direction = (enemy->game_object->transform->GetGlobalPosition() - transform->GetGlobalPosition()).Normalized();
				float3 knock_speed = -direction * enemy->stats["KnockBack"].GetValue();
				knock_speed.y = 0;

				ReceiveDamage(enemy->stats["Damage"].GetValue(), knock_speed, !enemy->is_mini);
				HUD->parent->GetComponent<UI_DamageCount>()->PlayerHasBeenHit(this);
				
				return;
			}
		}
	}
}

void PlayerController::StartImmune()
{
	is_immune = true;
	if (state->type == StateType::ROLLING && player_data.type == PlayerType::YENNEFER)
	{
		GameObject* meshes = game_object->GetChild("Meshes");
		controller->SetCollisionLayer("DashLayer");
		meshes->SetEnable(false);
	}
}

void PlayerController::StopImmune()
{
	is_immune = false;
	if (state->type == StateType::ROLLING && player_data.type == PlayerType::YENNEFER)
	{
		GameObject* meshes = game_object->GetChild("Meshes");
		controller->SetCollisionLayer("Player");
		meshes->SetEnable(true);
	}
}

void PlayerController::OnEnemyKill(uint enemy_type)
{
	if (player_data.type_kills.find(enemy_type) == player_data.type_kills.end()) //if this type was never killed create new entry
	{
		player_data.type_kills.insert(std::pair<uint, uint>(enemy_type, 0));
	}

	player_data.total_kills++;
	player_data.type_kills[enemy_type]++;

	//UI
	HUD->GetComponent<UI_Char_Frame>()->StartFadeKillCount(player_data.total_kills);
}

void PlayerController::OnDrawGizmosSelected()
{
	Gizmos::DrawWireSphere(transform->GetGlobalPosition(), player_data.revive_range, Color::Cyan()); //snap_range
	Gizmos::DrawWireSphere(transform->GetGlobalPosition(), battleCircle, Color::Green()); //battle circle
}
#pragma endregion Events
float3 PlayerController::GetDirectionVector()
{
	float3 direction_vector = Camera::GetCurrentCamera()->game_object_attached->transform->GetGlobalRotation().Mul(float3(movement_input.x, 0.f, movement_input.y).Normalized());
	direction_vector = Quat::RotateFromTo(Camera::GetCurrentCamera()->frustum.up, float3::unitY()) * direction_vector;

	return direction_vector;
}
#pragma region Init
void PlayerController::LoadStats()
{
	std::string character_string = player_data.type == PlayerController::PlayerType::GERALT ? "Geralt" : "Yennefer";
	std::string json_string = "GameData/Players/" + character_string + "Stats.json";

	JSONfilepack* json = JSONfilepack::GetJSON(json_string.c_str());
	JSONArraypack* stats_json = json->GetArray("Stats");

	Stat::FillStats(player_data.stats, stats_json);

	JSONfilepack::FreeJSON(json);
}
void PlayerController::InitKeyboardControls()
{
	if (controller_index == 1) {
		keyboard_move_up = SDL_SCANCODE_W;
		keyboard_move_left = SDL_SCANCODE_A;
		keyboard_move_right = SDL_SCANCODE_D;
		keyboard_move_down = SDL_SCANCODE_S;
		keyboard_jump = SDL_SCANCODE_SPACE;
		keyboard_dash = SDL_SCANCODE_LSHIFT;
		keyboard_light_attack = SDL_SCANCODE_C;
		keyboard_heavy_attack = SDL_SCANCODE_V;
		keyboard_revive = SDL_SCANCODE_F;
		keyboard_ultimate = SDL_SCANCODE_X;
		keyboard_spell_1 = SDL_SCANCODE_1;
		keyboard_spell_2 = SDL_SCANCODE_2;
		keyboard_spell_3 = SDL_SCANCODE_3;
		keyboard_spell_4 = SDL_SCANCODE_4;

		// HUD
		if(GameObject::FindWithName("HUD_Game"))
			HUD = GameObject::FindWithName("HUD_Game")->GetChild("UI_InGame")->GetChild("InGame")->GetChild("Character1");
	}
	else if (controller_index == 2) {
		keyboard_move_up = SDL_SCANCODE_UP;
		keyboard_move_left = SDL_SCANCODE_LEFT;
		keyboard_move_right = SDL_SCANCODE_RIGHT;
		keyboard_move_down = SDL_SCANCODE_DOWN;
		keyboard_jump = SDL_SCANCODE_RCTRL;
		keyboard_dash = SDL_SCANCODE_RSHIFT;
		keyboard_light_attack = SDL_SCANCODE_KP_0;
		keyboard_heavy_attack = SDL_SCANCODE_KP_PERIOD;
		keyboard_revive = SDL_SCANCODE_KP_ENTER;
		keyboard_ultimate = SDL_SCANCODE_KP_5;
		keyboard_spell_1 = SDL_SCANCODE_KP_1;
		keyboard_spell_2 = SDL_SCANCODE_KP_2;
		keyboard_spell_3 = SDL_SCANCODE_KP_3;
		keyboard_spell_4 = SDL_SCANCODE_KP_4;

		// HUD
		if(GameObject::FindWithName("HUD_Game"))
			HUD = GameObject::FindWithName("HUD_Game")->GetChild("UI_InGame")->GetChild("InGame")->GetChild("Character2");
	}
}
void PlayerController::CalculateAABB() {
	auto meshes = game_object->GetChild("Meshes")->GetComponentsInChildren<ComponentDeformableMesh>();
	max_aabb.SetNegativeInfinity();

	AABB new_section;
	for (auto i = meshes.begin(); i != meshes.end(); ++i) {
		new_section = (*i)->GetGlobalAABB();
		max_aabb.minPoint = {
			Maths::Min(new_section.minPoint.x, max_aabb.minPoint.x), Maths::Min(new_section.minPoint.y, max_aabb.minPoint.y),Maths::Min(new_section.minPoint.z, max_aabb.minPoint.z)
		};
		max_aabb.maxPoint = {
			Maths::Max(new_section.maxPoint.x, max_aabb.maxPoint.x), Maths::Max(new_section.maxPoint.y, max_aabb.maxPoint.y),Maths::Max(new_section.maxPoint.z, max_aabb.maxPoint.z)
		};
	}

	max_aabb.minPoint -= transform->GetGlobalPosition();
	max_aabb.maxPoint -= transform->GetGlobalPosition();
}
#pragma endregion Init
