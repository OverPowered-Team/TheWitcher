#include "GameManager.h"
#include "PlayerManager.h"
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

#include "Bonfire.h"
#include "Scores_Data.h"

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
	std::vector<GameObject*> particle_gos = game_object->GetChild("Particles")->GetChildren();
	for (auto it = particle_gos.begin(); it != particle_gos.end(); ++it) {
		particles.insert(std::pair((*it)->GetName(), (*it)));
		(*it)->SetEnable(false);
	}

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
}

void PlayerController::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_F1))
	{
		Effect* new_effect = GameManager::instance->effects_factory->CreateEffect("Geralt_Quen");
		AddEffect(new_effect);
	}

	UpdateInput();

	//State Machine--------------------------------------------------------
	State* new_state = !input_blocked? state->HandleInput(this): nullptr;
	if (new_state != nullptr)
		SwapState(new_state);

	state->Update(this);
	//---------------------------------------------------------------------

	//Effects-----------------------------
	EffectsUpdate();

	//MOVEMENT
	player_data.speed.y -= player_data.gravity * Time::GetDT();
	if(CheckBoundaries() && can_move)
		controller->Move(player_data.speed * Time::GetDT());

	if (controller->isGrounded) //RESET Y SPEED IF ON GROUND
	{
		player_data.speed.y = 0;
	}

	//Update animator variables
	animator->SetFloat("speed", float3(player_data.speed.x, 0, player_data.speed.z).Length());
	animator->SetBool("movement_input", mov_input);
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
	case StateType::CASTING:
		state = new CastingState();
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
	float3 direction_vector = Camera::GetCurrentCamera()->game_object_attached->transform->GetGlobalRotation().Mul(float3(movement_input.x, 0.f, movement_input.y).Normalized());

	direction_vector = (Quat::RotateFromTo(Camera::GetCurrentCamera()->frustum.up, float3::unitY()) * direction_vector).Normalized();
	float tmp_y = player_data.speed.y;
	player_data.speed = direction_vector * player_data.stats["Movement_Speed"].GetValue();
	player_data.speed.y = tmp_y;

	//rotate
	if (mov_input)
	{
		transform->SetGlobalRotation(Quat::RotateAxisAngle(float3::unitY(), atan2f(direction_vector.x, direction_vector.z)));
	}
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
					if (player_data.stats["Health"].GetValue() == 0)
					{
						shake->Shake(0.16f, 1, 5.f, 0.5f, 0.5f, 0.5f);
						Die();
					}
				}
			}
			if (particles[(*it)->name])
				particles[(*it)->name]->SetEnable(true);
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
		particles[attacks->GetCurrentAttack()->info.particle_name]->SetEnable(false);
		particles[attacks->GetCurrentAttack()->info.particle_name]->SetEnable(true);
	}
}

void PlayerController::PlayAllowParticle()
{
	if (attacks->GetCurrentAttack())
	{
		particles[attacks->GetCurrentAttack()->info.allow_combo_p_name]->SetEnable(false);
		particles[attacks->GetCurrentAttack()->info.allow_combo_p_name]->SetEnable(true);

	}
}

#pragma region PlayerActions
void PlayerController::Jump()
{
	player_data.speed.y = player_data.stats["Jump_Power"].GetValue();
	animator->PlayState("Air");
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
	is_immune = false;

	if(HUD)
		HUD->GetComponent<UI_Char_Frame>()->LifeChange(player_data.stats["Health"].GetValue(), player_data.stats["Health"].GetMaxValue());

	if(GameManager::instance->rumbler_manager)
		GameManager::instance->rumbler_manager->StartRumbler(RumblerType::REVIVE, controller_index);
	if(GameManager::instance->event_manager)
		GameManager::instance->event_manager->OnPlayerRevive(this);

	SetState(StateType::IDLE);
}

void PlayerController::ReceiveDamage(float dmg, float3 knock_speed)
{
	if (player_data.stats["Absorb"].GetValue() > 0)
	{
		AbsorbHit();
		return;
	}

	player_data.stats["Health"].DecreaseStat(dmg);

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
	else
	{
		animator->PlayState("Hit");
		player_data.speed = knock_speed;
		SetState(StateType::HIT);
	}

	if(GameManager::instance->rumbler_manager)
		GameManager::instance->rumbler_manager->StartRumbler(RumblerType::RECEIVE_HIT, controller_index);
}

void PlayerController::AbsorbHit()
{
	for (auto it = effects.begin(); it != effects.end();)
	{
		for (auto mods = (*it)->additive_modifiers.begin(); mods != (*it)->additive_modifiers.end(); ++mods)
		{
			if (mods->identifier == "Absorb")
			{
				RemoveEffect(it);
				return;
			}
		}
	}
}
#pragma endregion PlayerActions

void PlayerController::HitByRock(float time)
{
	//if is vulnerable and not already chiquito
		//life -= damage
	Invoke(std::bind(&PlayerController::RecoverFromRockHit, this), time);
	transform->SetLocalScale(1.f, 0.25f, 1.f);
}

void PlayerController::RecoverFromRockHit()
{
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

	if (dynamic_cast<AttackEffect*>(_effect) != nullptr)
	{
		attacks->OnAddAttackEffect(((AttackEffect*)_effect));
	}
	else
	{
		for (auto it = player_data.stats.begin(); it != player_data.stats.end(); ++it)
		{
			if(_effect->AffectsStat(it->second.name) && _effect->ticks_time == 0)
				it->second.ApplyEffect(_effect);
		}
	}

	GameObject* go = GameObject::Instantiate(_effect->vfx_on_apply.c_str(), {0, 0.5f, 0}, false, game_object);
	if (go)
		particles.insert(std::pair(_effect->vfx_on_apply, go));
}
std::vector<Effect*>::iterator PlayerController::RemoveEffect(std::vector<Effect*>::iterator it)
{
	Effect* tmp_effect = (*it);
	it = effects.erase(it);

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

	for (auto it = particles.begin(); it != particles.end();)
	{
		if (it->first == tmp_effect->vfx_on_apply)
		{
			GameObject::Destroy(it->second);
			it = particles.erase(it);
		}
		else
			++it;
	}

	delete tmp_effect;
	return it;
}
bool PlayerController::CheckBoundaries()
{
	float3 next_pos = transform->GetGlobalPosition() + player_data.speed * Time::GetDT();

	float3 moved = (next_pos - transform->GetGlobalPosition());
	AABB fake_aabb(max_aabb.minPoint + moved + transform->GetGlobalPosition(), max_aabb.maxPoint + moved + transform->GetGlobalPosition());

	float3 next_cam_pos = moved.Normalized() * 0.5 + camera->game_object_attached->transform->GetGlobalPosition(); // * 0.5 for middle point in camera

	Frustum fake_frustum = camera->frustum;
	fake_frustum.pos = next_cam_pos;

	CameraMovement* cam = (CameraMovement*)camera->game_object_attached->GetComponent<CameraMovement>();
	for (int i = 0; i < cam->players.size(); ++i)
	{
		if (cam->players[i] != this->game_object)
		{
			PlayerController* p = cam->players[i]->GetComponent<PlayerController>();
			if (p != nullptr) {
				AABB p_tmp = p->max_aabb;
				p_tmp.minPoint += cam->players[i]->transform->GetGlobalPosition();
				p_tmp.maxPoint += cam->players[i]->transform->GetGlobalPosition();

				if(camera->frustum.Contains(p_tmp) && !fake_frustum.Contains(p_tmp))
				{
					LOG("LEAVING BUDDY BEHIND");
					player_data.speed = float3::zero();
					return false;
				}
			}
		}
	}

	if (camera->frustum.Contains(fake_aabb)) {
		return true;
	}
	else {
		if (transform->GetGlobalPosition().Distance(fake_frustum.CenterPoint()) < next_pos.Distance(fake_frustum.CenterPoint())) {
			player_data.speed = float3::zero();
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
	animator->SetCurrentStateSpeed(0);
	is_immune = true;
	Invoke([this, speed]() -> void {this->RemoveFreeze(speed); }, freeze_time);
}

void PlayerController::RemoveFreeze(float speed)
{
	animator->SetCurrentStateSpeed(speed);
	is_immune = false;
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
				if (dash_coll->dash_particles[(*it)->name])
					dash_coll->dash_particles[(*it)->name]->SetEnable(true);
			}
		}
	}
}

void PlayerController::OnTriggerEnter(ComponentCollider* col)
{
	if ((strcmp("Bonfire", col->game_object_attached->GetName()) == 0))
	{
		Bonfire* bonfire = col->game_object_attached->GetComponent<Bonfire>();

		if (!Scores_Data::last_checkpoint_position.Equals(bonfire->checkpoint->transform->GetGlobalPosition()))
		{
			Scores_Data::last_checkpoint_position = bonfire->checkpoint->transform->GetGlobalPosition();
			HUD->parent->parent->GetComponent<InGame_UI>()->ShowCheckpointSaved();
		}

		if (bonfire->is_active && !bonfire->HaveThisPlayerUsedThis(this))
		{
			if (player_data.stats["Health"].GetMaxValue() > player_data.stats["Health"].GetValue()
				|| player_data.stats["Chaos"].GetMaxValue() > player_data.stats["Chaos"].GetValue())
			{
				// Heal
				player_data.stats["Health"].IncreaseStat(player_data.stats["Health"].GetMaxValue());
				player_data.stats["Chaos"].IncreaseStat(player_data.stats["Chaos"].GetMaxValue());
				HUD->GetComponent<UI_Char_Frame>()->LifeChange(player_data.stats["Health"].GetValue(), player_data.stats["Health"].GetMaxValue());
				HUD->GetComponent<UI_Char_Frame>()->ManaChange(player_data.stats["Chaos"].GetValue(), player_data.stats["Chaos"].GetMaxValue());


				// Player Used this Bonfire
				bonfire->SetBonfireUsed(this);
			}
		}
	}

	if (!godmode)
	{
		if (strcmp(col->game_object_attached->GetTag(), "EnemyAttack") == 0) {

			auto comps = col->game_object_attached->parent->GetComponents<Alien>();

			for (auto i = comps.begin(); i != comps.end(); ++i) {
				Enemy* enemy = dynamic_cast<Enemy*>(*i);
				if (enemy) {
					//Calculate Knockback
					float3 direction = (enemy->game_object->transform->GetGlobalPosition() - transform->GetGlobalPosition()).Normalized();
					float3 knock_speed = -direction * enemy->stats["KnockBack"].GetValue();
					knock_speed.y = 0;

					ReceiveDamage(enemy->stats["Damage"].GetValue(), knock_speed);
					return;
				}
			}
		}
	}
}

void PlayerController::OnEnemyKill()
{
	player_data.total_kills++;
	GameManager::instance->player_manager->IncreaseUltimateCharge(10);
}

void PlayerController::OnUltimateActivation(float value)
{
	animator->IncreaseAllStateSpeeds(2.0f);
}

void PlayerController::OnUltimateDeactivation(float value)
{
	animator->DecreaseAllStateSpeeds(2.0f);
}
void PlayerController::OnDrawGizmosSelected()
{
	Gizmos::DrawWireSphere(transform->GetGlobalPosition(), player_data.revive_range, Color::Cyan()); //snap_range
}
#pragma endregion Events

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
		keyboard_dash = SDL_SCANCODE_LALT;
		keyboard_light_attack = SDL_SCANCODE_V;
		keyboard_heavy_attack = SDL_SCANCODE_B;
		keyboard_revive = SDL_SCANCODE_C;
		keyboard_ultimate = SDL_SCANCODE_X;
		keyboard_spell = SDL_SCANCODE_F;

		// HUD
		if(GameObject::FindWithName("HUD_Game"))
			HUD = GameObject::FindWithName("HUD_Game")->GetChild("UI_InGame")->GetChild("InGame")->GetChild("Character1");
	}
	else if (controller_index == 2) {
		keyboard_move_up = SDL_SCANCODE_I;
		keyboard_move_left = SDL_SCANCODE_J;
		keyboard_move_right = SDL_SCANCODE_L;
		keyboard_move_down = SDL_SCANCODE_K;
		keyboard_jump = SDL_SCANCODE_RSHIFT;
		keyboard_dash = SDL_SCANCODE_RALT;
		keyboard_light_attack = SDL_SCANCODE_RCTRL;
		keyboard_heavy_attack = SDL_SCANCODE_RIGHTBRACKET;
		keyboard_revive = SDL_SCANCODE_M;
		keyboard_ultimate = SDL_SCANCODE_APOSTROPHE;
		keyboard_spell = SDL_SCANCODE_COMMA;

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
