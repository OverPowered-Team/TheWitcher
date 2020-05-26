#include "MoveCameraArroundObject.h"

MoveCameraArroundObject::MoveCameraArroundObject() : Alien()
{
}

MoveCameraArroundObject::~MoveCameraArroundObject()
{
}

void MoveCameraArroundObject::Start()
{
	if (path)
	{
		curve = path->GetComponent<ComponentCurve>();
		if(curve!=nullptr)
			LOG("Curve found!");
	}
}

void MoveCameraArroundObject::Update()
{
	if (curve)
	{
		float3 currentPos = curve->curve.ValueAt(current_pos);
		LOG("%f", current_pos);
		float3 nextPos = curve->curve.ValueAt(current_pos + speed * Time::GetDT() * 5);
		float3 railVector = (currentPos - nextPos).Normalized();
		game_object->transform->SetGlobalPosition(currentPos);
		current_pos += speed * Time::GetDT();
		if (current_pos > 1)
		{
			current_pos = 0;
		}
	}
}