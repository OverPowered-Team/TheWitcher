#include "RotateObject.h"

RotateObject::RotateObject() : Alien()
{
}

RotateObject::~RotateObject()
{
}

void RotateObject::Start()
{
}

void RotateObject::Update()
{
	yrotation += Time::GetDT() * speed;
	transform->SetGlobalRotation(Quat::FromEulerXYZ(0, yrotation, 0));
	if (yrotation > 360)
	{
		yrotation = 0;
	}
}