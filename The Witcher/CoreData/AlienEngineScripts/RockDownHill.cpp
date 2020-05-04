#include "RockDownHill.h"
#include "PlayerController.h"

RockDownHill::RockDownHill() : Alien()
{
}

RockDownHill::~RockDownHill()
{
}

void RockDownHill::Start()
{
	rb = GetComponent<ComponentRigidBody>();
	emitter = GetComponent<ComponentAudioEmitter>();
	timer = Time::GetGameTime();
}

void RockDownHill::Update()
{
	rb->SetPosition(transform->GetGlobalPosition() + direction * speed * Time::GetDT());
	rb->SetRotation(transform->GetGlobalRotation() * Quat::RotateAxisAngle(axis_rot, rot_speed * Time::GetDT()));

	Doppler(); 

	if (Time::GetGameTime() - timer >= time)
		Destroy(game_object);
}

void RockDownHill::CalculateDirection(const float3& end_pos)
{
	direction = (end_pos - transform->GetGlobalPosition()).Normalized();
	axis_rot = Cross(float3::unitY(), direction);
}

void RockDownHill::SetMoveAndRotationSpeed(float rot_speed, float speed, float time, float dmg, float time_smash)
{
	this->rot_speed = rot_speed;
	this->speed = speed;
	this->time = time;
	this->damage = dmg;
	this->time_smashed = time_smash;
}

void RockDownHill::OnTriggerEnter(ComponentCollider* trigger)
{
	if (strcmp(trigger->game_object_attached->GetTag(), "RockEnd") == 0) {
		GameObject::Destroy(game_object);
	}
	else if (strcmp(trigger->game_object_attached->GetTag(), "Player") == 0) {
		PlayerController* player_ctrl = trigger->game_object_attached->GetComponent<PlayerController>();
		if (player_ctrl && !player_ctrl->is_immune)
		{
			player_ctrl->HitByRock(time_smashed, damage);
		}
	}
}

void RockDownHill::OnCollisionEnter(const Collision& trigger)
{
	if (strcmp(trigger.game_object->GetTag(), "RockEnd") == 0) {
		GameObject::Destroy(game_object);
	}
}

void RockDownHill::Doppler()
{
	// https://www.audiokinetic.com/qa/1175/how-can-i-create-a-doppler-effects-with-wwise
	
	static float distToListenner = 0.0f, lastDistToListenner = 0.0f, approachSpeed = 0.0f, pitchMulti = 0.f; 

	distToListenner = (Camera::GetCurrentCamera()->GetCameraPosition()
		- game_object->GetComponentTransform()->GetGlobalPosition()).Length(); 


	approachSpeed = distToListenner - lastDistToListenner; 

	
	// TODO: RTPC
	// emitter->SetRTPCValue("DopplerRocks", approachSpeed); 


	lastDistToListenner = distToListenner; 
}
