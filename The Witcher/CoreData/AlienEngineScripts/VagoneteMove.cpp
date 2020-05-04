#include "VagoneteMove.h"

void VagoneteMove::Start()
{
	curve = GameObject::FindWithName("Curve")->GetComponent<ComponentCurve>();
}

void VagoneteMove::Update()
{
	static float actual_pos = 0;

	transform->SetLocalPosition(curve->curve.ValueAt(actual_pos));

	Quat quat = transform->GetLocalRotation();
	Quat quat2 = Quat::RotateFromTo(transform->up, curve->curve.NormalAt(actual_pos));

	transform->SetLocalRotation(quat2);
	actual_pos += speed;

	if (Input::GetKeyDown(SDL_SCANCODE_1)) {
		actual_pos = 0;
	}
}