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
	rot = rot * inclinationRot;

	if (Input::GetKeyRepeat(SDL_SCANCODE_LEFT)) {
		currentInclination2 -= speedInclination * Time::GetDT();
		if (currentInclination2 < -inclination4player) {
			currentInclination2 = -inclination4player;
		}
		rot = rot * Quat::RotateX(currentInclination2 * Maths::Deg2Rad());
	}
	else if (Input::GetKeyRepeat(SDL_SCANCODE_RIGHT)) {
		currentInclination2 += speedInclination * Time::GetDT();
		if (currentInclination2 > inclination4player) {
			currentInclination2 = inclination4player;
		}
		rot = rot * Quat::RotateX(currentInclination2 * Maths::Deg2Rad());
	}
	else {
		if (currentInclination2 < 0) {
			currentInclination2 += speedInclination * Time::GetDT();
			if (currentInclination2 > 0) {
				currentInclination2 = 0;
			}
			rot = rot * Quat::RotateX(currentInclination2 * Maths::Deg2Rad());
		}
		else if (currentInclination2 > 0) {
			currentInclination2 -= speedInclination * Time::GetDT();
			if (currentInclination2 < 0) {
				currentInclination2 = 0;
			}
			rot = rot * Quat::RotateX(currentInclination2 * Maths::Deg2Rad());
		}
	}

	if (Input::GetKeyRepeat(SDL_SCANCODE_A)) {
		currentInclination1 -= speedInclination * Time::GetDT();
		if (currentInclination1 < -inclination4player) {
			currentInclination1 = -inclination4player;
		}
		rot = rot * Quat::RotateX(currentInclination1 * Maths::Deg2Rad());
	}
	else if (Input::GetKeyRepeat(SDL_SCANCODE_D)) {
		currentInclination1 += speedInclination * Time::GetDT();
		if (currentInclination1 > inclination4player) {
			currentInclination1 = inclination4player;
		}
		rot = rot * Quat::RotateX(currentInclination1 * Maths::Deg2Rad());
	}
	else {
		if (currentInclination1 < 0) {
			currentInclination1 += speedInclination * Time::GetDT();
			if (currentInclination1 > 0) {
				currentInclination1 = 0;
			}
			rot = rot * Quat::RotateX(currentInclination1 * Maths::Deg2Rad());
		}
		else if (currentInclination1 > 0) {
			currentInclination1 -= speedInclination * Time::GetDT();
			if (currentInclination1 < 0) {
				currentInclination1 = 0;
			}
			rot = rot * Quat::RotateX(currentInclination1 * Maths::Deg2Rad());
		}
	}

	transform->SetLocalRotation(rot);
	transform->SetLocalPosition(currentPos);

	actual_pos += speed * Time::GetDT();

	if (Input::GetKeyDown(SDL_SCANCODE_1)) {
		actual_pos = 0;
	}
}