#include "RockSpawner.h"
#include "RockDownHill.h"

RockSpawner::RockSpawner() : Alien()
{
}

RockSpawner::~RockSpawner()
{
}

void RockSpawner::Start()
{
	FindChildren();
	direction = spawn_end->transform->GetGlobalPosition() - spawn_0->transform->GetGlobalPosition();
	time_btw = Random::GetRandomFloatBetweenTwo(min_time_btw, max_time_btw);
}

void RockSpawner::FindChildren()
{
	spawn_0 = game_object->GetChild("RockSpawn_0");
	spawn_1 = game_object->GetChild("RockSpawn_1");
	spawn_end = game_object->GetChild("RockSpawn_end");
}

void RockSpawner::Update()
{
	if (enabled)
		if (Time::GetGameTime() - timer >= time_btw) {
			timer = Time::GetGameTime();
			time_btw = Random::GetRandomFloatBetweenTwo(min_time_btw, max_time_btw);
			float3 new_pos = GetRandomPositionBtw(spawn_0->transform->GetGlobalPosition(), spawn_1->transform->GetGlobalPosition());
			RockDownHill* obj = GameObject::Instantiate(rock, new_pos, false, game_object)->GetComponent<RockDownHill>();
			obj->CalculateDirection(new_pos + direction);
			obj->SetMoveAndRotationSpeed(rocks_rotation, rocks_speed);
		}
}

void RockSpawner::Disable()
{
	enabled = false;
}

void RockSpawner::OnDrawGizmos() {
	if (spawn_0 == nullptr)
		FindChildren();

	Gizmos::DrawSphere(spawn_0->transform->GetGlobalPosition(), 0.3f, Color::Green());
	Gizmos::DrawSphere(spawn_1->transform->GetGlobalPosition(), 0.3f, Color::Green());
	Gizmos::DrawSphere(spawn_end->transform->GetGlobalPosition(), 0.3f, Color::Red());

	Gizmos::DrawLine(spawn_0->transform->GetGlobalPosition(), spawn_1->transform->GetGlobalPosition(), Color::Cyan());
	Gizmos::DrawLine(spawn_0->transform->GetGlobalPosition(), spawn_end->transform->GetGlobalPosition(), Color::Cyan());

	Gizmos::DrawLine(spawn_1->transform->GetGlobalPosition(), spawn_end->transform->GetGlobalPosition() + (spawn_1->transform->GetGlobalPosition() - spawn_0->transform->GetGlobalPosition()), Color::Cyan());
}

float3 RockSpawner::GetRandomPositionBtw(const float3& min, const float3& max)
{
	return float3(Random::GetRandomFloatBetweenTwo(min.x, max.x), Random::GetRandomFloatBetweenTwo(min.y, max.y), Random::GetRandomFloatBetweenTwo(min.z, max.z));
}
