#include "PlayerControllerTest.h"

PlayerControllerTest::PlayerControllerTest() : Alien()
{
}

PlayerControllerTest::~PlayerControllerTest()
{
}

void PlayerControllerTest::Start()
{
	ccontroller = (ComponentCharacterController*)GetComponent(ComponentType::CHARACTER_CONTROLLER);
}

void PlayerControllerTest::Update()
{
	float axisX = Input::GetControllerHoritzontalLeftAxis(controllerIndex);
	float axisY = Input::GetControllerVerticalLeftAxis(controllerIndex);
	//float angle = atan2f(axisY, axisX);
	float3 movement = float3(axisX, 0.f, axisY) * movementSpeed * Time::GetDT();
	ccontroller->SetWalkDirection(movement);

	if (Input::GetControllerButtonDown(controllerIndex, Input::CONTROLLER_BUTTON_A)) {
		ccontroller->Jump(transform->up * jumpPower);
	}
}