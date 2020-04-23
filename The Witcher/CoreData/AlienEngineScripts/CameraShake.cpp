#include "CameraShake.h"

CameraShake::CameraShake() : Alien()
{
}

CameraShake::~CameraShake()
{
}

void CameraShake::Start()
{
	preQuat = transform->GetGlobalRotation();
}

void CameraShake::Update()
{
	static float jiji = 1.f;
	if (Input::GetKeyDown(SDL_SCANCODE_RETURN)) {
		jiji -= 0.08f;
		Time::SetScaleTime(jiji);
	}

	if (Input::GetKeyDown(SDL_SCANCODE_P)) {
		jiji = 1.f;
		Time::SetScaleTime(jiji);
	}

	if (Input::GetKeyDown(SDL_SCANCODE_S)) {
		Shake(0.13f);
	}

	if (Input::GetKeyDown(SDL_SCANCODE_A)) {
		RepeatShake(0.13f, 0.f, 0.2f, 50.f);
	}

	if(time_to_stop >= 0.0)
		if (Time::GetGameTime() - invoke_timer >= time_to_stop) {
			CancelInvoke();
			time_to_stop = 0.0f;
		}

	if (trauma > 0) {
		post_off_set += off_set * Time::GetDT();

		transform->SetGlobalRotation(preQuat);
		float3 rot = transform->GetGlobalRotation().ToEulerXYZ();
		float yaw = rot.x + maxYaw * trauma *(Maths::PerlinNoise(0, post_off_set, 0.8f, 0.8f) * 2 - 1);
		float pitch = rot.y + maxPitch* trauma * (Maths::PerlinNoise(1, 0.8f, post_off_set, 0.8f) * 2 - 1);
		float roll = rot.z + maxRoll* trauma * (Maths::PerlinNoise(2, 0.8, 0.8f, post_off_set) * 2 - 1);

		transform->SetGlobalRotation(Quat::FromEulerXYZ(yaw, pitch, roll));
		trauma -= Time::GetDT() * traumaDecay * (trauma + 0.3f);
	}
	else {
		preQuat = transform->GetGlobalRotation();
		post_off_set = off_set;
		traumaDecay = traumaDecayDef;
	}
}

void CameraShake::Shake(float strength, const float& traumaDecay)
{
	strength = Maths::Min(strength, 1.f);

	this->traumaDecay = traumaDecay;
	trauma += strength;
	trauma = Maths::Max(0.f, Maths::Min(trauma, 1.f));
}

void CameraShake::Shake(float strength)
{
	Shake(strength, traumaDecayDef);
}

void CameraShake::RepeatShake(float strength, float seconds_to_first_invoke, float seconds_between_invokes, float time_to_stop)
{
	this->time_to_stop = time_to_stop;
	invoke_timer = Time::GetGameTime();
	InvokeRepeating([strength, this]() -> void {this->Shake(strength); }, seconds_to_first_invoke, seconds_between_invokes);
}