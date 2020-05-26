#include "Physics.h"
#include "Application.h"
#include "ModulePhysX.h"

int Physics::GetLayerMask(const char* layer)
{
	int index = 0;

	if (App->physx->layers.GetIndexByName(layer, index))
	{
		return 1 << index;
	}

	return -1;
}

int Physics::GetLayerMask(const vector<const char*>& layers)
{
	int index = 0;
	int layer_mask = -1;
	for (int i = 0; i < layers.size(); ++i)
	{
		if (App->physx->layers.GetIndexByName(layers[i], index))
		{
			if (layer_mask == -1)
				layer_mask = 1 << index;
			else
				layer_mask = layer_mask | (1 << index);
		}
	}
	return layer_mask;
}

void Physics::SetGravity(const float3 gravity)
{
	App->physx->SetGravity(gravity);
}

float3 Physics::GetGravity()
{
	return App->physx->GetGravity();
}

bool Physics::Raycast(float3 origin, float3 unit_dir, float max_dist, int layer_mask)
{
	return App->physx->Raycast(origin, unit_dir, max_dist, layer_mask);
}

bool Physics::Raycast(float3 origin, float3 unit_dir, float max_dist, RaycastHit& hit, int layer_mask)
{
	return  App->physx->Raycast(origin, unit_dir, max_dist, hit, layer_mask);
}

const std::vector<RaycastHit>& Physics::RaycastAll(float3 origin, float3 unit_dir, float max_dist, int layer_mask)
{
	static std::vector<RaycastHit> ret;
	ret = App->physx->RaycastAll(origin, unit_dir, max_dist, layer_mask);
	return ret;
}

bool Physics::CapsuleCast(float4x4 trans, float height, float radius, float3 unit_dir, float max_dist, int layer_mask)
{
	return App->physx->CapsuleCast(trans, height, radius, unit_dir, max_dist, layer_mask);
}

bool Physics::CapsuleCast(float4x4 trans, float height, float radius, float3 unit_dir, float max_dist, RaycastHit& hit, int layer_mask)
{
	return App->physx->CapsuleCast(trans, height, radius, unit_dir, max_dist, hit, layer_mask);
}

const vector<RaycastHit>& Physics::CapsuleCastAll(float4x4 trans, float height, float radius, float3 unit_dir, float max_dist, int layer_mask)
{
	static std::vector<RaycastHit> ret;
	ret = App->physx->CapsuleCastAll(trans, height, radius, unit_dir, max_dist, layer_mask);
	return ret;
}

const std::vector<ComponentCollider*>& Physics::OverlapSphere(float3 center, float radius, int layer_mask)
{
	static  std::vector<ComponentCollider*> ret;
	ret = App->physx->OverlapSphere(center, radius, layer_mask);
	return ret;
}
