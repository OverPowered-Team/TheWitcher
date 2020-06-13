#include "Dwarf.h"
#include "GameManager.h"
#include "PlayerManager.h"
#include "PlayerController.h"

void Dwarf::Update()
{
	auto pos_p1 = GameManager::instance->player_manager->players[0]->transform->GetGlobalPosition();
	auto pos_p2 = GameManager::instance->player_manager->players[1]->transform->GetGlobalPosition();
	auto dist_1 = game_object->transform->GetGlobalPosition() - pos_p1;
	auto dist_2 = game_object->transform->GetGlobalPosition() - pos_p2;

	float3 to_look = (dist_1.Length() < dist_2.Length()) ? pos_p1 : pos_p2;
	auto direction = to_look - game_object->transform->GetGlobalPosition();
	if (direction.Length() >= radius)
		return;

	float3 velocity_vec = direction.Normalized() * speed * Time::GetDT();
	float angle = atan2f(velocity_vec.z, velocity_vec.x);
	Quat rot = Quat::RotateAxisAngle(float3::unitY(), -(angle * Maths::Rad2Deg() - 90.f) * Maths::Deg2Rad());

	for(int i = 0; i<game_object->GetChildren().size() - 1; ++i)
		if(game_object->GetChild(i)->GetName() != "Dwarf_Barrel1")
			game_object->GetChild(i)->transform->SetGlobalRotation(rot);
}
