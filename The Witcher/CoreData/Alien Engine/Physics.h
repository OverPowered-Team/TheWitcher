#pragma once
#include <vector>
#include "MathGeoLib/include/Math/float3.h"
#include "UtilitiesPhysX.h"

//class ComponentCollider;
typedef unsigned int uint;

class __declspec(dllexport) Physics {
public:

	/*static void SetGravity(const float3 gravity);
	static float3 GetGravity();*/

	static bool Raycast(float3 origin, float3 unitDir, float maxDistance, PxRaycastBuffer& hit); // TODO: make own pxraycastbuffer data to abstract from physx
	static const std::vector<PxRaycastHit>& RaycastAll(float3 origin, float3 unitDir, float maxDistance);
	static const std::vector<ComponentCollider*>& OverlapSphere(float3 center, float radius);
};