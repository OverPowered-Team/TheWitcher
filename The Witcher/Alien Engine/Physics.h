#pragma once
#include <vector>
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Geometry/Ray.h"

class ComponentCollider;

class __declspec(dllexport) Physics {
public:

	static void SetGravity(const float3 gravity);
	static float3 GetGravity();
	static std::vector<ComponentCollider*> RayCastAll(Ray ray);
	static ComponentCollider* RayCastClosest(math::Ray ray);
	static std::vector<ComponentCollider*>  SphereCast(float3 position, float radius);

};

