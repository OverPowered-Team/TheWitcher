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
	float angle = atan2f(vector.y, math::Sqrt((vector.z * vector.z) + (vector.x * vector.x)));

	float3 normal = curve->curve.NormalAt(actual_pos);

	Quat myDesiredRotUp = Quat::RotateFromTo(Quat::identity().WorldY(), normal);
	Quat myDesiredRotForward = Quat::RotateZ(angle);

	transform->SetLocalRotation(myDesiredRotForward * myDesiredRotUp);
	transform->SetLocalPosition(currentPos);

	actual_pos += (1/(float)curve->curve.detail) * Time::GetDT() * speed;

	if (Input::GetKeyDown(SDL_SCANCODE_1)) {
		actual_pos = 0;
	}
}