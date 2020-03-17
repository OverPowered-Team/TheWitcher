#include "PlayerAttacks.h"
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
	rbody = (ComponentRigidBody*)GetComponent(ComponentType::RIGID_BODY);
	attacks = (PlayerAttacks*)GetComponentScript("PlayerAttacks");
}

void PlayerController::Update()
{

	/*---------------CONTROLLER-----------------------*/
	HandleMovement();

	/*---------------CONTROLLER-----------------------*/

	switch (state)
	{
	case PlayerController::PlayerState::IDLE: {

		if (Input::GetControllerButtonDown(controllerIndex, Input::CONTROLLER_BUTTON_X)) {
			attacks->SaveInput('X');
			attacks->StartAttack();
			state = PlayerState::BASIC_ATTACK;
		}
		else if(Input::GetControllerButtonDown(controllerIndex, Input::CONTROLLER_BUTTON_Y)) {
			attacks->SaveInput('Y');
			attacks->StartAttack();
			state = PlayerState::BASIC_ATTACK;
		}

		if (Input::GetControllerButtonDown(controllerIndex, Input::CONTROLLER_BUTTON_RIGHTSHOULDER)) {
			animator->PlayState("Roll");
			state = PlayerState::DASHING;
		}

	} break;
	case PlayerController::PlayerState::RUNNING:
	{
		if (Input::GetControllerButtonDown(controllerIndex, Input::CONTROLLER_BUTTON_X)) {
			animator->PlayState("Attack");
			state = PlayerState::BASIC_ATTACK;
		}

		if (Input::GetControllerButtonDown(controllerIndex, Input::CONTROLLER_BUTTON_RIGHTSHOULDER)) {
			animator->PlayState("Roll");
			state = PlayerState::DASHING;
		}

	} break;
	case PlayerController::PlayerState::BASIC_ATTACK:
	{
		if (Input::GetControllerButtonDown(controllerIndex, Input::CONTROLLER_BUTTON_X))
			attacks->SaveInput('X');
		else if (Input::GetControllerButtonDown(controllerIndex, Input::CONTROLLER_BUTTON_Y))
			attacks->SaveInput('Y');

		attacks->UpdateAttack();
	} break;
	case PlayerController::PlayerState::JUMPING:
		break;
	case PlayerController::PlayerState::DASHING:
		break;
	case PlayerController::PlayerState::MAX:
		break;
	default:
		break;
	}

	/*---------------KEYBOARD-----------------------*/

	if (state == PlayerState::RUNNING && abs(playerData.currentSpeed) < 0.1F)
		state = PlayerState::IDLE;

	if (state == PlayerState::IDLE && abs(playerData.currentSpeed) > 0.1F)
		state = PlayerState::RUNNING;

	playerData.currentSpeed = 0;
}

void PlayerController::HandleMovement()
{
	float axisX = Input::GetControllerHoritzontalLeftAxis(controllerIndex);
	float axisY = Input::GetControllerVerticalLeftAxis(controllerIndex);

	float speed = (abs(axisX) + abs(axisY)) * 0.5F;

	float3 vector = float3(axisX, 0.f, axisY);
	vector = Camera::GetCurrentCamera()->game_object_attached->transform->GetGlobalRotation().Mul(vector).Normalized();
	float angle = atan2f(vector.z, vector.x);
	Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90) * Maths::Deg2Rad());

	if (abs(axisX) >= stick_threshold || abs(axisY) >= stick_threshold) {
		playerData.currentSpeed = (playerData.movementSpeed * speed * Time::GetDT());
		transform->SetLocalRotation(rot);
	}

	if (abs(playerData.currentSpeed) > 0) {
		rbody->SetVelocity(transform->forward * playerData.currentSpeed * 100.f);
	}
	else {
		rbody->SetVelocity({ 0, rbody->GetVelocity().y, 0 });
		rbody->SetAngularVelocity({ 0, 0, 0 });
	}

	animator->SetFloat("speed", Maths::Abs(playerData.currentSpeed));
}

void PlayerController::OnAnimationEnd(const char* name) {
	if (strcmp(name, "Attack") == 0)
		state = PlayerState::IDLE;

	if (strcmp(name, "Roll") == 0)
		state = PlayerState::IDLE;
}