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
}

void PlayerController::Update()
{

	/*---------------CONTROLLER-----------------------*/
	if (can_move)
		HandleMovement();

	/*---------------CONTROLLER-----------------------*/

	switch (state)
	{
	case PlayerController::PlayerState::IDLE: {

		can_move = true;

		if (Input::GetControllerButtonDown(controller_index, Input::CONTROLLER_BUTTON_X)) {
			animator->PlayState("Attack");
			state = PlayerState::BASIC_ATTACK;
			can_move = false;
		}

		if (Input::GetControllerButtonDown(controller_index, Input::CONTROLLER_BUTTON_RIGHTSHOULDER)) {
			animator->PlayState("Roll");
			state = PlayerState::DASHING;
			//ccontroller->ApplyImpulse(transform->forward.Normalized() * 20);

		}

		if (Input::GetControllerButtonDown(controller_index, Input::CONTROLLER_BUTTON_A)) {
			state = PlayerState::JUMPING;
			animator->PlayState("Jump");
			if (ccontroller->CanJump()) {
				ccontroller->Jump(transform->up * player_data.jump_power);
				animator->SetBool("air", true);
			}
		}

	} break;
	case PlayerController::PlayerState::RUNNING:
	{
		can_move = true;

		if (Input::GetControllerButtonDown(controller_index, Input::CONTROLLER_BUTTON_X)) {
			animator->PlayState("Attack");
			state = PlayerState::BASIC_ATTACK;
		}

		if (Input::GetControllerButtonDown(controller_index, Input::CONTROLLER_BUTTON_RIGHTSHOULDER)) {
			animator->PlayState("Roll");
			state = PlayerState::DASHING;
			//ccontroller->ApplyImpulse(transform->forward.Normalized() * 20);
		}

		if (Input::GetControllerButtonDown(controller_index, Input::CONTROLLER_BUTTON_A)) {
			state = PlayerState::JUMPING;
			animator->PlayState("Jump");
			if (ccontroller->CanJump()) {
				ccontroller->Jump(transform->up * player_data.jump_power);
				animator->SetBool("air", true);
			}
		}

	} break;
	case PlayerController::PlayerState::BASIC_ATTACK:
		ccontroller->SetWalkDirection(float3::zero());
		can_move = false;
		break;
	case PlayerController::PlayerState::JUMPING:
		can_move = true;
		if (ccontroller->CanJump())
			animator->SetBool("air", false);
		break;
	case PlayerController::PlayerState::DASHING:
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

void PlayerController::HandleMovement()
{
	float2 joystickInput = float2(
		Input::GetControllerHoritzontalLeftAxis(controller_index),
		Input::GetControllerVerticalLeftAxis(controller_index));
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

void PlayerController::OnAnimationEnd(const char* name) {
	if (strcmp(name, "Attack") == 0)
		state = PlayerState::IDLE;

	if (strcmp(name, "Roll") == 0) {
		state = PlayerState::IDLE;
	}
}