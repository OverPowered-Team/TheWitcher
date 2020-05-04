#include "VagoneteMove.h"

void VagoneteMove::Start()
{
	curve = GameObject::FindWithName("Curve")->GetComponent<ComponentCurve>();
}

void VagoneteMove::Update()
{
	float3 currentPos = curve->curve.ValueAt(actual_pos);
	float3 nextPos = curve->curve.ValueAt(actual_pos + ((1 / (float)curve->curve.detail) * Time::GetDT() * speed) * 10);
	
	float3 vector = (nextPos - currentPos).Normalized();

	float3 normal = curve->curve.NormalAt(actual_pos).Normalized();
	float3 Y = vector.Cross(normal);

	float3x3 rot = float3x3(vector, normal, Y);

	transform->SetLocalRotation(rot.ToQuat());
	transform->SetLocalPosition(currentPos);

	actual_pos += (1/(float)curve->curve.detail) * Time::GetDT() * speed;

	if (Input::GetKeyDown(SDL_SCANCODE_1)) {
		actual_pos = 0;
	}
}