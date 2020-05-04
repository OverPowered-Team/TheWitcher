#include "VagoneteMove.h"

void VagoneteMove::Start()
{
	curve = GameObject::FindWithName("Curve")->GetComponent<ComponentCurve>();
}

void VagoneteMove::Update()
{
	static float actual_pos = 0;

	transform->SetLocalPosition(curve->curve.ValueAt(actual_pos));

	Quat myRot = transform->GetLocalRotation();
	float3 normal = curve->curve.NormalAt(actual_pos);

	Quat myDesiredRotUp = Quat::RotateFromTo(transform->up, normal);

	transform->SetLocalRotation(myDesiredRotUp * myRot);


	actual_pos += (1/(float)curve->curve.detail) * Time::GetDT() * speed;

	if (Input::GetKeyDown(SDL_SCANCODE_1)) {
		actual_pos = 0;
	}
}