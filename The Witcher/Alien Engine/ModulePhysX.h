#pragma once

#include "Module.h"
#include "UtilitiesPhysX.h"
#include "CollisionLayers.h"

using namespace physx;
typedef ComponentCollider* Collider;
typedef vector<RaycastHit> RaycastHits;
typedef vector<Collider> Colliders;

#define BUFFER_SIZE 64
#define DEFAULT_LAYER_MASK -1

class ModulePhysX : public Module
{
	friend class PanelPhysics;
	friend class ModuleObjects;
	friend class ComponentPhysics;
	friend class ComponentBasePhysic;

	friend class ComponentCollider;
	friend class ComponentBoxCollider;
	friend class ComponentSphereCollider;
	friend class ComponentCapsuleCollider;
	friend class ComponentMeshCollider;
	friend class ComponentConvexHullCollider;
	friend class ComponentCharacterController;

	friend class ComponentJoint;
	friend class ComponentConfigurableJoint;


public:

	ModulePhysX(bool start_enabled = true);
	~ModulePhysX();

	void SetGravity(float3 gravity);
	float3 GetGravity();

	// Queries -----------------------------------------
	enum QueryType { RAYCAST, SWEEPCAST, OVERLAP, NO_TYPE };

	bool			ClosestPoint(float3 point, float3& closest_point, Collider collider);
	bool 			ClosestPoint(float3 point , float3& closest_point, Collider collider, float3 position, Quat rotation);

	bool			Raycast(float3 origin, float3 unit_dir, float max_distance, int layer_mask);
	bool			Raycast(float3 origin, float3 unit_dir, float max_distance, RaycastHit& hit, int layer_mask);
	RaycastHits		RaycastAll(float3 origin, float3 unitDir, float maxDistance, int layer_mask);

	bool			CheckBox(float4x4& trans, float half_size, int layer_mask);
	bool			BoxCast(float4x4& trans, float half_size, float3 unit_dir, float max_dist, int layer_mask);
	bool			BoxCast(float4x4& trans, float half_size, float3 unit_dir, float max_dist, RaycastHit& hit, int layer_mask);
	RaycastHits		BoxCastAll(float4x4 trans, float half_size, float3 unit_dir, float max_dist, int layer_mask);
	Colliders		OverlapBox(float4x4& trans, float half_size, int layer_mask);
	
	bool			CheckSphere(float3 center, float radius, int layer_mask);	
	bool			SphereCast(float3 center, float radius, float3 unit_dir, float max_dist, int layer_mask);
	bool			SphereCast(float3 center, float radius, float3 unit_dir, float max_dist, RaycastHit& hit, int layer_mask);
	RaycastHits		SphereCastAll(float3 center, float radius, float3 unit_dir, float max_dist, int layer_mask);
	Colliders		OverlapSphere(float3 center, float radius, int layer_mask);

	bool			CheckCapsule(float4x4& trans, float height, float radius, int layer_mask);
	bool			CapsuleCast(float4x4& trans, float height, float radius, float3 unit_dir, float max_dist, int layer_mask);
	bool			CapsuleCast(float4x4& trans, float height, float radius, float3 unit_dir, float max_dist, RaycastHit& hit, int layer_mask);
	RaycastHits		CapsuleCastAll(float4x4 trans, float height, float radius, float3 unit_dir, float max_dist, int layer_mask);
	Colliders		OverlapCapsule(float4x4& trans, float height, float radius, int layer_mask);

private:



	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	// Queries -----------------------------------------

	PxHitFlags GetHitFlags();
	PxQueryFilterData GetFilterData(bool any_hit);
	void BeginQueryFilter(QueryType query_type, int layer_mask,  bool generate_vector = false);
	void EndQueryFilter();

	bool SweepAny(PxGeometry& geometry, float4x4& trans, float3& unit_dir, float max_dist, int layer_mask);
	bool Sweep(PxGeometry& geometry, float4x4& trans, float3& unit_dir, float max_dist, RaycastHit& hit,  int layer_mask);
	void SweepAll(PxGeometry& geometry, float4x4& trans, float3& unit_dir, float max_dist, RaycastHits& hits, int layer_mask);

	bool Check(PxGeometry& geometry, float4x4& trans, int layer_mask);
	void Overlap(PxGeometry& geometry, float4x4& trans, Colliders& colliders, int layer_mask);

	void DebugDrawConvex(const float4x4& transform, const float3& scale , const PxConvexMesh* mesh, const float3& color = float3::one()) const;
	void DrawCollider(ComponentCollider* collider);
	void DrawWorld();

	PxRigidActor* CreateBody(const float4x4& transform, bool is_dynamic);
	void RemoveBody(PxRigidActor* body);
	PxShape* CreateShape(const PxGeometry& geometry, const PxMaterial& material);
	PxController* CreateCharacterController(PxControllerDesc& desc);
	PxController* GetController(uint index) const;
	uint GetNbControllers() const;
	PxMaterial* CreateMaterial(float staticFriction = 0.5f, float dynamicFriction = 0.5f, float restitution = 0.5f) const;

	// Delay Libraries -------------------------------------

	bool LoadPhysicsExplicitely();
	void UnloadPhysicsExplicitely();

public:
	QueryType query_type = QueryType::NO_TYPE;
	
	CollisionLayers	layers;
	bool debug_physics = false;
	bool mouse_pick_colliders = true;
	bool mouse_pick_triggers = true;

	int	layer_mask = DEFAULT_LAYER_MASK;
	int  multiple_hit = false;
	bool query_hit_triggers = true;
	bool query_hit_backfaces = false;
	bool query_initial_overlap = true;

private:

	float accumulator = 0.f;
	float3 gravity = float3(0.f, -9.81, 0.f);

	HMODULE foundation_lib = nullptr;
	HMODULE common_lib = nullptr;
	HMODULE physx_lib = nullptr;
	HMODULE cooking_lib = nullptr;

	const char* foundation_lib_path = APP_BIN_DIR DLL_DIR "PhysXFoundation_" DLL_NAME_BITS ".dll";
	const char* common_lib_path = APP_BIN_DIR DLL_DIR "PhysXCommon_" DLL_NAME_BITS ".dll";
	const char* physx_lib_path = APP_BIN_DIR DLL_DIR "PhysX_" DLL_NAME_BITS ".dll";
	const char* gpu_lib_path = APP_BIN_DIR DLL_DIR "PhysXGpu_" DLL_NAME_BITS ".dll";
	const char* cooking_lib_path = APP_BIN_DIR DLL_DIR "PhysXCooking_" DLL_NAME_BITS ".dll";

	// delay load hooks
	CustomDelayLoadHook			delayLoadHook;
	CustomGpuLoadHook			gpuLoadHook;
	
	PxDefaultAllocator			px_allocator;
	CustomErrorCallback			px_error_callback;
	SimulationEventCallback*    px_simulation_callback = nullptr;
	ControllerFilterCallback*	px_controller_filter = nullptr;
	QueryFilterCallback*		px_query_filter = nullptr;

	PxFoundation*				px_foundation = nullptr;
	PxPhysics*					px_physics = nullptr;
	PxCooking*					px_cooking = nullptr;
	PxDefaultCpuDispatcher*		px_dispatcher = nullptr;
	PxScene*					px_scene = nullptr;
	PxPvd*						px_pvd = nullptr;

	PxControllerManager*		controllers_manager = nullptr;
};