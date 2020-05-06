#include "Physics.h"
#include "Application.h"
#include "ModulePhysX.h"

void Physics::SetGravity(const float3 gravity)
{
	App->physx->SetGravity(gravity);
}

float3 Physics::GetGravity()
{
	return App->physx->GetGravity();
}

bool Physics::Raycast(float3 origin, float3 unit_dir, float max_dist)
{
	return  App->physx->Raycast(origin, unit_dir, max_dist);
}

bool Physics::Raycast(float3 origin, float3 unit_dir, float max_dist, RaycastHit& hit)
{
	return  App->physx->Raycast(origin, unit_dir, max_dist, hit);
}

const std::vector<RaycastHit>& Physics::RaycastAll(float3 origin, float3 unitDir, float maxDistance)
{
	static std::vector<RaycastHit> ret;
	ret = App->physx->RaycastAll(origin, unitDir, maxDistance);
	return ret;
}

// TODO: uncomment this when we have heap change integred
const std::vector<ComponentCollider*>& Physics::OverlapSphere(float3 center, float radius)
{
	static  std::vector<ComponentCollider*> ret;
	ret = App->physx->OverlapSphere(center, radius);
	return ret;
}
