#pragma once
#include <vector>
#include "MathGeoLib/include/Math/float3.h"
#include "UtilitiesPhysX.h"

using namespace std;
typedef unsigned int uint;

class __declspec(dllexport) Physics {
public:

	static void								  SetGravity(const float3 gravity);
	static float3							  GetGravity();
	static bool								  Raycast(float3 origin, float3 unit_dir, float max_dist); // TODO: make own pxraycastbuffer data to abstract from physx
	static bool								  Raycast(float3 origin, float3 unit_dir, float max_dist, RaycastHit& hit); // TODO: make own pxraycastbuffer data to abstract from physx
	static const vector<RaycastHit>&		  RaycastAll(float3 origin, float3 unit_dir, float max_dist);
	static const vector<ComponentCollider*>&  OverlapSphere(float3 center, float radius);
};