#include "ControllerTest.h"

ControllerTest::ControllerTest() : Alien()
{
}

ControllerTest::~ControllerTest()
{
}

void ControllerTest::Start()
{
}

void ControllerTest::Update()
{
	float2 axis = { Input::GetControllerHoritzontalLeftAxis(controllerIndex), Input::GetControllerVerticalLeftAxis(controllerIndex) };

	if (!axis.IsZero()) {
		float3 current_position = transform->GetLocalPosition();
		transform->SetLocalPosition({ current_position.x + speed * axis.x * Time::GetDT(), current_position.y, current_position.z + speed * axis.y * Time::GetDT() });
	}
	if (Input::GetControllerButtonDown(controllerIndex, Input::CONTROLLER_BUTTON_X)) {
		speed = 20;
	}
}