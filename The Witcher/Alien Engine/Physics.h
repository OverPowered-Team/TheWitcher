#pragma once
#include <vector>
#include "MathGeoLib/include/Math/float3.h"
#include "UtilitiesPhysX.h"

using namespace std;
typedef unsigned int uint;

class __declspec(dllexport) Physics {
public:

	static int							      GetLayerMask( const char* layer);
	static int							      GetLayerMask( const vector<const char*>& layers);
	static void								  SetGravity(const float3 gravity);
	static float3							  GetGravity();

	static bool								  Raycast(float3 origin, float3 unit_dir, float max_dist, int layer_mask = -1); 
	static bool								  Raycast(float3 origin, float3 unit_dir, float max_dist, RaycastHit& hit, int layer_mask = -1); 
	static const vector<RaycastHit>&		  RaycastAll(float3 origin, float3 unit_dir, float max_dist, int layer_mask = -1);

	static bool								  CapsuleCast(float4x4 trans, float height, float radius, float3 unit_dir, float max_dist, int layer_mask = -1);
	static bool								  CapsuleCast(float4x4 trans, float height, float radius, float3 unit_dir, float max_dist, RaycastHit& hit, int layer_mask = -1);
	static const vector<RaycastHit>&          CapsuleCastAll(float4x4 trans, float height, float radius, float3 unit_dir, float max_dist, int layer_mask = -1);

	static const vector<ComponentCollider*>&  OverlapSphere(float3 center, float radius, int layer_mask = -1);
};