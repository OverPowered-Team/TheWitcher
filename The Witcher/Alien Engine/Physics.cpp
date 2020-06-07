#include "Physics.h"
#include "Application.h"
#include "ModulePhysX.h"

#include "ComponentCollider.h"
#include "ComponentRigidBody.h"
#include "ComponentTransform.h"
#include "ComponentPhysics.h"


// General ----------------------------------------

float3 Physics::GetGravity()
{
	return App->physx->GetGravity();
}
void Physics::SetGravity(const float3 gravity)
{
	App->physx->SetGravity(gravity);
}
void Physics::SetQueryInitialOverlaping(bool value)
{
	App->physx->query_initial_overlap = value;
}
void Physics::SetQueryHitTriggers(bool value)
{
	App->physx->query_hit_triggers = value;
}
void Physics::SetQueryHitBackfaces(bool value)
{
	App->physx->query_hit_backfaces = value;
}

// Layers -----------------------------------------

void Physics::IgnoreLayerColision(int layer_0, int layer_1, bool value)
{
	App->physx->layers.SetLayers(layer_0, layer_1, value);
}

bool Physics::GetIgnoreLayerColision(int layer_0, int layer_1)
{
	if (App->physx->layers.GetNameByIndex(layer_0) && App->physx->layers.GetNameByIndex(layer_1)) 
		return App->physx->layers.data[layer_0][layer_1];

	return false;
}

int Physics::GetLayerMask(const char* layer)
{
	int index = 0;
	if (App->physx->layers.GetIndexByName(layer, index)) {
		return 1 << index;
	}
	return -1;
}
int Physics::GetLayerMask(vector<const char*>& layers)
{
	int index = 0;
	int layer_mask = -1;

	for (int i = 0; i < layers.size(); ++i) {

		if (App->physx->layers.GetIndexByName(layers[i], index)) {
			if (layer_mask == -1)
				layer_mask = (1<<index);
			else
				layer_mask |= (1<<index);
		}
	}

	return layer_mask;
}

// Queries -----------------------------------------

bool Physics::ClosestPoint(float3 point, float3& closest_point, Collider collider)
{
	return App->physx->ClosestPoint(point, closest_point, collider);
}
bool Physics::ClosestPoint(float3 point, float3& closest_point, Collider collider, float3 position, Quat rotation)
{
	return App->physx->ClosestPoint(point, closest_point, collider, position, rotation);
}

// ------------------ Ray ------------------------

bool Physics::Raycast(float3 origin, float3 unit_dir, float max_dist, int layer_mask)
{
	return App->physx->Raycast(origin, unit_dir, max_dist, layer_mask);
}
bool Physics::Raycast(float3 origin, float3 unit_dir, float max_dist, RaycastHit& hit, int layer_mask)
{
	return  App->physx->Raycast(origin, unit_dir, max_dist, hit, layer_mask);
}
const RaycastHits& Physics::RaycastAll(float3 origin, float3 unit_dir, float max_dist, int layer_mask)
{
	static RaycastHits ret;
	ret = App->physx->RaycastAll(origin, unit_dir, max_dist, layer_mask);
	return ret;
}

// ------------------ Box -------------------------

bool Physics::CheckBox(float4x4& trans, float half_size, int layer_mask)
{
	return App->physx->CheckBox(trans, half_size, layer_mask);
}
bool Physics::BoxCast(float4x4& trans, float half_size, float3 unit_dir, float max_dist, int layer_mask)
{
	return App->physx->BoxCast(trans, half_size, unit_dir, max_dist, layer_mask);
}
bool Physics::BoxCast(float4x4& trans, float half_size, float3 unit_dir, float max_dist, RaycastHit& hit, int layer_mask)
{
	return App->physx->BoxCast(trans, half_size, unit_dir, max_dist, hit, layer_mask);
}
const RaycastHits& Physics::BoxCastAll(float4x4 trans, float half_size, float3 unit_dir, float max_dist, int layer_mask)
{
	static RaycastHits ret;
	ret = App->physx->BoxCastAll(trans, half_size, unit_dir, max_dist, layer_mask);
	return ret;
}
const Colliders& Physics::OverlapBox(float4x4& trans, float half_size, int layer_mask)
{
	static Colliders ret;
	ret = App->physx->OverlapBox(trans, half_size, layer_mask);
	return ret;
}

// ------------------ Sphere -------------------------

bool Physics::CheckSphere(float3 center, float radius, int layer_mask)
{
	return App->physx->CheckSphere(center, radius, layer_mask);
}
bool Physics::SphereCast(float3 center, float radius, float3 unit_dir, float max_dist, int layer_mask)
{
	return  App->physx->SphereCast(center, radius, unit_dir, max_dist, layer_mask);
}
bool Physics::SphereCast(float3 center, float radius, float3 unit_dir, float max_dist, RaycastHit& hit, int layer_mask)
{
	return  App->physx->SphereCast(center, radius, unit_dir, max_dist, hit, layer_mask);
}
const RaycastHits& Physics::SphereCastAll(float3 center, float radius, float3 unit_dir, float max_dist, int layer_mask)
{
	static RaycastHits ret;
	ret = App->physx->SphereCastAll(center, radius, unit_dir, max_dist, layer_mask);
	return ret;
}
const Colliders& Physics::OverlapSphere(float3 center, float radius, int layer_mask)
{
	static Colliders ret;
	ret = App->physx->OverlapSphere(center, radius, layer_mask);
	return ret;
}

// ------------------ Capsule ------------------------

bool Physics::CheckCapsule(float4x4& trans, float height, float radius, int layer_mask)
{
	return App->physx->CheckCapsule(trans, height, radius, layer_mask);;
}
bool Physics::CapsuleCast(float4x4& trans, float height, float radius, float3 unit_dir, float max_dist, int layer_mask)
{
	return App->physx->CapsuleCast(trans, height, radius, unit_dir, max_dist, layer_mask);
}
bool Physics::CapsuleCast(float4x4& trans, float height, float radius, float3 unit_dir, float max_dist, RaycastHit& hit, int layer_mask)
{
	return App->physx->CapsuleCast(trans, height, radius, unit_dir, max_dist, hit, layer_mask);
}
const RaycastHits& Physics::CapsuleCastAll(float4x4 trans, float height, float radius, float3 unit_dir, float max_dist, int layer_mask)
{
	static RaycastHits ret;
	ret = App->physx->CapsuleCastAll(trans, height, radius, unit_dir, max_dist, layer_mask);
	return ret;
}
const Colliders& Physics::OverlapCapsule(float4x4& trans, float height, float radius, int layer_mask)
{
	static Colliders ret;
	ret = App->physx->OverlapCapsule(trans, height, radius, layer_mask);
	return ret;
}
