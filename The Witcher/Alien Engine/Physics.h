#pragma once
#include <vector>
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Geometry/Ray.h"

class ComponentCollider;
typedef unsigned int uint;

class __declspec(dllexport) Physics {
public:

	static void SetGravity(const float3 gravity);
	static float3 GetGravity();

	static ComponentCollider*			   RayCastClosest(math::Ray ray);
	static std::vector<ComponentCollider*> RayCastAll(Ray ray);
	static std::vector<ComponentCollider*> SphereCast(float3 position, float radius);
	static std::vector<ComponentCollider*> BoxCast(float3 size, float3 position, Quat rotation);
};

