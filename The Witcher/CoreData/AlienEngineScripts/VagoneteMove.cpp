#include "VagoneteMove.h"

void VagoneteMove::Start()
{
	curve = GameObject::FindWithName("Curve")->GetComponent<ComponentCurve>();
}

void VagoneteMove::Update()
{
	float3 currentPos = curve->curve.ValueAt(actual_pos);
	float3 nextPos = curve->curve.ValueAt(actual_pos + speed * Time::GetDT() * 5);

	//Pitch (slope)
	float3 railVector = (currentPos - nextPos).Normalized();
	Quat rot = Quat::LookAt(float3::unitX(), railVector, float3::unitY(), float3::unitY());

	//Inclination (normals + players)
	float3 inclinationVector = curve->curve.NormalAt(actual_pos).Normalized();
	Quat inclinationRot = Quat::RotateFromTo(float3::unitY(), inclinationVector);
	inclinationRot.Inverse();

	transform->SetLocalRotation(rot * inclinationRot);
	transform->SetLocalPosition(currentPos);

	//APPROACH 3
	//float3 vector = (nextPos - currentPos).Normalized();

	//float3 normal = curve->curve.NormalAt(actual_pos).Normalized();
	//float3 Y = vector.Cross(normal);

	//float3x3 rot = float3x3(vector, normal, Y);

	//transform->SetLocalRotation(rot.ToQuat());
	//transform->SetLocalPosition(currentPos);

	actual_pos += speed * Time::GetDT();

	if (Input::GetKeyDown(SDL_SCANCODE_1)) {
		actual_pos = 0;
	}
}