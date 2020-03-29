#include "PlayerController.h"
#include "RelicBehaviour.h"
#include "Effect.h"

PlayerController::PlayerController() : Alien()
{
}

PlayerController::~PlayerController()
{
}

void PlayerController::Start()
{
	animator = (ComponentAnimator*)GetComponent(ComponentType::ANIMATOR);
	controller = (ComponentCharacterController*)GetComponent(ComponentType::CHARACTER_CONTROLLER);

	c_run = (ComponentParticleSystem*)p_run->GetComponent(ComponentType::PARTICLES);
	c_attack = (ComponentParticleSystem*)p_attack->GetComponent(ComponentType::PARTICLES);
	c_spell = (ComponentParticleSystem*)p_spell->GetComponent(ComponentType::PARTICLES);

	audio = (ComponentAudioEmitter*)GetComponent(ComponentType::A_EMITTER);

	frustum = &Camera::GetCurrentCamera()->frustum;
	
	c_run->GetSystem()->StopEmmitter();
	c_attack->GetSystem()->Stop();
	c_spell->GetSystem()->StopEmmitter();

	if (controller_index == 1) {
		keyboard_move_up = SDL_SCANCODE_W;
		keyboard_move_left = SDL_SCANCODE_A;
		keyboard_move_right = SDL_SCANCODE_D;
		keyboard_move_down = SDL_SCANCODE_S;
		keyboard_jump = SDL_SCANCODE_SPACE;
		keyboard_dash = SDL_SCANCODE_LALT;
		keyboard_light_attack = SDL_SCANCODE_V;
	}
	else if (controller_index == 2) {
		keyboard_move_up = SDL_SCANCODE_I;
		keyboard_move_left = SDL_SCANCODE_J;
		keyboard_move_right = SDL_SCANCODE_L;
		keyboard_move_down = SDL_SCANCODE_K;
		keyboard_jump = SDL_SCANCODE_RSHIFT;
		keyboard_dash = SDL_SCANCODE_RALT;
		keyboard_light_attack = SDL_SCANCODE_RCTRL;
	}
}

void PlayerController::Update()
{
	float2 joystickInput = float2(
		Input::GetControllerHoritzontalLeftAxis(controller_index),
		Input::GetControllerVerticalLeftAxis(controller_index));

	LOG("JOYSTICK %.3f, %.3f", joystickInput.x, joystickInput.y);
	
	if (joystickInput.Length() > 0) {
		if (CheckBoundaries(joystickInput)) {
			if (can_move) {
				HandleMovement(joystickInput);
			}
		}
	}
	else if (AnyKeyboardInput())
	{
		if (can_move) {
			float2 keyboardInput = float2(0.f, 0.f);
			if (Input::GetKeyRepeat(keyboard_move_left)) {
				keyboardInput.x += 1.f;
			}
			if (Input::GetKeyRepeat(keyboard_move_right)) {
				keyboardInput.x += -1.f;
			}
			if (Input::GetKeyRepeat(keyboard_move_up)) {
				keyboardInput.y += 1.f;
			}
			if (Input::GetKeyRepeat(keyboard_move_down)) {
				keyboardInput.y += -1.f;
			}
			LOG("KEYBOARD %.3f, %.3f", keyboardInput.x, keyboardInput.y);

			if (CheckBoundaries(keyboardInput)) {
				if (can_move) {
					HandleMovement(keyboardInput);
				}
			}
		}
	}

	switch (state)
	{
	case PlayerController::PlayerState::IDLE: {

		can_move = true;
		c_run->GetSystem()->StopEmmitter();
		if (Input::GetControllerButtonDown(controller_index, controller_attack)
			|| Input::GetKeyDown(keyboard_light_attack)) {
			animator->PlayState("Attack");
			state = PlayerState::BASIC_ATTACK;
			audio->StartSound("Hit_Sword");
			can_move = false;
		}

		if (Input::GetControllerButtonDown(controller_index, controller_dash)
			|| Input::GetKeyDown(keyboard_dash)) {
			animator->PlayState("Roll");
			state = PlayerState::DASHING;
			//ccontroller->ApplyImpulse(transform->forward.Normalized() * 20);
		}

		if (Input::GetControllerButtonDown(controller_index, controller_spell)
			|| Input::GetKeyDown(keyboard_spell)) {
			animator->PlayState("Spell");
			state = PlayerState::CASTING;
		}

		if (Input::GetControllerButtonDown(controller_index, controller_jump)
			|| Input::GetKeyDown(keyboard_jump)) {
			state = PlayerState::JUMPING;
			animator->PlayState("Air");
			if (controller->CanJump()) {
				controller->Jump(transform->up * player_data.jump_power);
				animator->SetBool("air", true);
			}
		}

	} break;
	case PlayerController::PlayerState::RUNNING:
	{
		c_run->GetSystem()->StartEmmitter();
		can_move = true;

		if (Time::GetGameTime() - timer >= delay_footsteps) {
			timer = Time::GetGameTime();
			audio->StartSound();
		}

		if (Input::GetControllerButtonDown(controller_index, controller_attack)
			|| Input::GetKeyDown(keyboard_light_attack)) {
			animator->PlayState("Attack");
			state = PlayerState::BASIC_ATTACK;
			audio->StartSound("Hit_Sword");
		}

		if (Input::GetControllerButtonDown(controller_index, controller_dash)
			|| Input::GetKeyDown(keyboard_dash)) {
			animator->PlayState("Roll");
			state = PlayerState::DASHING;
			//ccontroller->ApplyImpulse(transform->forward.Normalized() * 20);
		}

		if (Input::GetControllerButtonDown(controller_index, controller_spell)
			|| Input::GetKeyDown(keyboard_spell)) {
			animator->PlayState("Spell");
			state = PlayerState::CASTING;
		}

		if (Input::GetControllerButtonDown(controller_index, controller_jump)
			|| Input::GetKeyDown(keyboard_jump)) {
			state = PlayerState::JUMPING;
			animator->PlayState("Air");
			if (controller->CanJump()) {
				controller->Jump(transform->up * player_data.jump_power);
				animator->SetBool("air", true);
			}
		}

	} break;
	case PlayerController::PlayerState::BASIC_ATTACK:
		c_run->GetSystem()->StopEmmitter();
		c_attack->GetSystem()->Restart();
		controller->SetWalkDirection(float3::zero());
		can_move = false;
		break;
	case PlayerController::PlayerState::JUMPING:
		c_run->GetSystem()->StopEmmitter();
		can_move = true;
		if (controller->CanJump())
			animator->SetBool("air", false);
		break;
	case PlayerController::PlayerState::DASHING:
		c_run->GetSystem()->StopEmmitter();
		can_move = false;
		break;
	case PlayerController::PlayerState::CASTING:
		c_run->GetSystem()->StopEmmitter();
		controller->SetWalkDirection(float3::zero());
		can_move = false;
		break;
	case PlayerController::PlayerState::MAX:
		break;
	default:
		break;
	}

	/*---------------KEYBOARD-----------------------*/

	if (state == PlayerState::RUNNING && abs(player_data.currentSpeed) < 0.05F)
		state = PlayerState::IDLE;

	if (state == PlayerState::IDLE && abs(player_data.currentSpeed) > 0.05F) {
		state = PlayerState::RUNNING;
		audio->StartSound();
		timer = Time::GetGameTime();
	}

	if (state == PlayerState::JUMPING && controller->CanJump()) {
		if (abs(player_data.currentSpeed) < 0.1F)
			state = PlayerState::IDLE;
		if (abs(player_data.currentSpeed) > 0.1F)
			state = PlayerState::RUNNING;
	}
	player_data.currentSpeed = 0;
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

void PlayerController::OnDrawGizmos()
{
	Gizmos::DrawCube(float3::zero(), float3::one(), Color::Purple());
	Gizmos::DrawCube(transform->GetGlobalPosition(), float3::one(), Color::Purple());
}

void PlayerController::HandleMovement(float2 joystickInput)
{
	float joystickIntensity = joystickInput.Length();

	float3 vector = float3(joystickInput.x, 0.f, joystickInput.y);
	vector = Camera::GetCurrentCamera()->game_object_attached->transform->GetGlobalRotation().Mul(vector);
	vector.y = 0.f;
	vector.Normalize();

	float angle = atan2f(vector.z, vector.x);
	Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());

	if (abs(joystickInput.x) >= stick_threshold || abs(joystickInput.y) >= stick_threshold)
	{
		player_data.currentSpeed = (player_data.movementSpeed * joystickIntensity * Time::GetDT());
		controller->SetRotation(rot);
	}

	if (state == PlayerState::DASHING)
	{
		controller->SetWalkDirection(transform->forward.Normalized() * player_data.movementSpeed * player_data.dash_power * Time::GetDT());
	}
	else
	{
		controller->SetWalkDirection(vector * player_data.currentSpeed);
	}

	animator->SetFloat("speed", Maths::Abs(player_data.currentSpeed));
}

void PlayerController::OnAttackEffect()
{
	c_attack->Restart();
}

void PlayerController::OnAnimationEnd(const char* name) {

	LOG("entro acabar %s", name);
	if (strcmp(name, "Attack") == 0) {
		if (abs(player_data.currentSpeed) < 0.1F)
			state = PlayerState::IDLE;
		if (abs(player_data.currentSpeed) > 0.1F)
			state = PlayerState::RUNNING;
	}

	if (strcmp(name, "Roll") == 0) {
		if(abs(player_data.currentSpeed) < 0.1F)
			state = PlayerState::IDLE;
		if (abs(player_data.currentSpeed) > 0.1F)
			state = PlayerState::RUNNING;
	}

	if (strcmp(name, "Spell") == 0) {
		if (abs(player_data.currentSpeed) < 0.1F)
			state = PlayerState::IDLE;
		if (abs(player_data.currentSpeed) > 0.1F)
			state = PlayerState::RUNNING;
	}
}

void PlayerController::PlaySpell()
{
	c_spell->Restart();
}

void PlayerController::PickUpRelic(Relic* _relic)
{
	relics.push_back(_relic);

	for (int i = 0; i < _relic->effects.size(); i++)
	{
		AddEffect(_relic->effects.at(i));
	}
}

void PlayerController::AddEffect(Effect* _effect)
{

}

bool PlayerController::CheckBoundaries(const float2& joystickInput)
{
	float3 next_pos = float3::zero();
	float joystickIntensity = joystickInput.Length();

	float3 vector = float3(joystickInput.x, 0.f, joystickInput.y);
	vector = Camera::GetCurrentCamera()->game_object_attached->transform->GetGlobalRotation().Mul(vector);
	vector.y = 0.f;
	vector.Normalize();

	float angle = atan2f(vector.z, vector.x);
	Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());

	float speed = 0.f;

	if (abs(joystickInput.x) >= stick_threshold || abs(joystickInput.y) >= stick_threshold)
	{
		speed = (player_data.movementSpeed * joystickIntensity * Time::GetDT());
	}

	if (state == PlayerState::DASHING)
	{
		next_pos = transform->GetGlobalPosition() + transform->forward.Normalized() * player_data.movementSpeed * player_data.dash_power * Time::GetDT();
		animator->SetFloat("speed", Maths::Abs(player_data.currentSpeed));
	}
	else
	{
		next_pos = transform->GetGlobalPosition() + vector * speed * 2.f;
		LOG("vector %.2f, %.2f, %.2f", vector.x, vector.y, vector.z);
	}

	AABB aabb = AABB(next_pos, next_pos + float3(1, 1, 1));
	if (cube_test != nullptr)
		cube_test->transform->SetGlobalPosition(next_pos);

	LOG("NEXT POS %.2f, %.2f, %.2f", next_pos.x, next_pos.y, next_pos.z);
	LOG("%.2f, %.2f, %.2f", aabb.CenterPoint().x, aabb.CenterPoint().y, aabb.CenterPoint().z);

	if (frustum->Contains(aabb)) {
		return true;
	}
	else {
		controller->SetWalkDirection(float3::zero());
		LOG("Player %i is not inside camera frustum", controller_index);
		return false;
	}
}
