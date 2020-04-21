#include "Physics.h"
#include "Application.h"
#include "ModulePhysX.h"

bool Physics::Raycast(float3 origin, float3 unitDir, float maxDistance, PxRaycastBuffer& hit)
{
	return App->physx->Raycast(origin, unitDir, maxDistance, hit);
}

const std::vector<PxRaycastHit>& Physics::RaycastAll(float3 origin, float3 unitDir, float maxDistance)
{
	static std::vector<PxRaycastHit> hits;
	hits = App->physx->RaycastAll(origin, unitDir, maxDistance);
	return hits;
}

// TODO: uncomment this when we have heap change integred
const std::vector<ComponentCollider*>& Physics::OverlapSphere(float3 center, float radius)
{
	static std::vector<ComponentCollider*> colliders;
	colliders = App->physx->OverlapSphere(center, radius);
	return colliders;
}

//void Physics::SetGravity(const float3 gravity)
//{
//	App->physics->SetGravity(gravity);
//}
//
//float3 Physics::GetGravity()
//{
//	return App->physics->GetGravity();
//}
