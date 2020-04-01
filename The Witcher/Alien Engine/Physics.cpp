#include "Physics.h"
#include "Application.h"
#include "ModulePhysics.h"

void Physics::SetGravity(const float3 gravity)
{
	App->physics->SetGravity(gravity);
}

float3 Physics::GetGravity()
{
	return App->physics->GetGravity();
}

uint Physics::RayCastAll(Ray ray, ComponentCollider*** comp_array)
{
	std::vector<ComponentCollider*> found = App->physics->RayCastAll(ray);
	if (!found.empty()) {
		(*comp_array) = new ComponentCollider * [found.size()];
		for (uint i = 0; i < found.size(); ++i) {
			(*comp_array)[i] = found[i];
		}
	}
	return found.size();
}

ComponentCollider* Physics::RayCastClosest(math::Ray ray)
{
	return App->physics->RayCastClosest(ray);
}

uint Physics::SphereCast(float3 position, float radius, ComponentCollider*** comp_array)
{
	std::vector<ComponentCollider*> found = App->physics->SphereCast(position, radius);
	if (!found.empty()) {
		(*comp_array) = new ComponentCollider *[found.size()];
		for (uint i = 0; i < found.size(); ++i) {
			(*comp_array)[i] = found[i];
		}
	}
	return found.size();
}

uint Physics::BoxCast(float3 size, float3 position, Quat rotation, ComponentCollider*** comp_array)
{
	auto found = App->physics->BoxCast(size, position, rotation);
	if (!found.empty()) {
		(*comp_array) = new ComponentCollider * [found.size()];
		for (uint i = 0; i < found.size(); ++i) {
			(*comp_array)[i] = found[i];
		}
	}
	return found.size();
}

void Physics::FreeArray(ComponentCollider*** comp_array)
{
	delete[] * comp_array;
}
