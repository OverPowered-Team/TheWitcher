#include "PlayerController.h"

PlayerController::PlayerController() : Alien()
{
}

PlayerController::~PlayerController()
{
}

void PlayerController::Start()
{
	animator = (ComponentAnimator*)GetComponent(ComponentType::ANIMATOR);
	ccontroller = (ComponentCharacterController*)GetComponent(ComponentType::CHARACTER_CONTROLLER);

	c_run = (ComponentParticleSystem*)p_run->GetComponent(ComponentType::PARTICLES);
	c_attack = (ComponentParticleSystem*)p_attack->GetComponent(ComponentType::PARTICLES);

	c_run->GetSystem()->StopEmmitter();
	c_attack->GetSystem()->Stop();


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

	if (joystickInput.Length() > 0) {
		keyboard_input = false;
	}
	else if (Input::GetKeyDown(keyboard_move_up)
		|| Input::GetKeyDown(keyboard_move_down)
		|| Input::GetKeyDown(keyboard_move_left)
		|| Input::GetKeyDown(keyboard_move_right)
		|| Input::GetKeyDown(keyboard_dash)
		|| Input::GetKeyDown(keyboard_jump))
	{
		keyboard_input = true;
	}

	if (can_move)
	{
		if (!keyboard_input) {
			HandleMovement(joystickInput);
		}
		else {
			float2 keyboardInput = float2(0.f, 0.f);
			if (Input::GetKeyRepeat(keyboard_move_left)) {
				keyboardInput.x += 1.f;
			}
			if (Input::GetKeyRepeat(keyboard_move_right)) {
				keyboardInput.x -= 1.f;
			}
			if (Input::GetKeyRepeat(keyboard_move_up)) {
				keyboardInput.y += 1.f;
			}
			if (Input::GetKeyRepeat(keyboard_move_down)) {
				keyboardInput.y -= 1.f;
			}
			HandleMovement(keyboardInput);
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
			if (ccontroller->CanJump()) {
				ccontroller->Jump(transform->up * player_data.jump_power);
				animator->SetBool("air", true);
			}
		}

	} break;
	case PlayerController::PlayerState::RUNNING:
	{
		c_run->GetSystem()->StartEmmitter();
		can_move = true;
		
		if (Input::GetControllerButtonDown(controller_index, controller_attack)
			|| Input::GetKeyDown(keyboard_light_attack)) {
			animator->PlayState("Attack");
			state = PlayerState::BASIC_ATTACK;
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
			if (ccontroller->CanJump()) {
				ccontroller->Jump(transform->up * player_data.jump_power);
				animator->SetBool("air", true);
			}
		}

	} break;
	case PlayerController::PlayerState::BASIC_ATTACK:
		c_run->GetSystem()->StopEmmitter();
		//c_attack->GetSystem()->Restart();
		ccontroller->SetWalkDirection(float3::zero());
		can_move = false;
		break;
	case PlayerController::PlayerState::JUMPING:
		c_run->GetSystem()->StopEmmitter();
		can_move = true;
		if (ccontroller->CanJump())
			animator->SetBool("air", false);
		break;
	case PlayerController::PlayerState::DASHING:
		c_run->GetSystem()->StopEmmitter();
		can_move = false;
		break;
	case PlayerController::PlayerState::CASTING:
		c_run->GetSystem()->StopEmmitter();
		ccontroller->SetWalkDirection(float3::zero());
		can_move = false;
		break;
	case PlayerController::PlayerState::MAX:
		break;
	default:
		break;
	}

	/*---------------KEYBOARD-----------------------*/

	if (state == PlayerState::RUNNING && abs(player_data.currentSpeed) < 0.1F)
		state = PlayerState::IDLE;

	if (state == PlayerState::IDLE && abs(player_data.currentSpeed) > 0.1F)
		state = PlayerState::RUNNING;

	if (state == PlayerState::JUMPING && ccontroller->CanJump()) {
		if (abs(player_data.currentSpeed) < 0.1F)
			state = PlayerState::IDLE;
		if (abs(player_data.currentSpeed) > 0.1F)
			state = PlayerState::RUNNING;
	}

	player_data.currentSpeed = 0;
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
		ccontroller->SetRotation(rot);
	}

	if (state == PlayerState::DASHING)
	{
		ccontroller->SetWalkDirection(transform->forward.Normalized() * player_data.movementSpeed * player_data.dash_power * Time::GetDT());
	}
	else
	{
		ccontroller->SetWalkDirection(vector * player_data.currentSpeed);
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