#pragma once

#include "Module.h"
//#include "PxPhysicsAPI.h"
#include "UtilitiesPhysX.h"
#include <wtypes.h>
#include "CollisionLayers.h"

using namespace physx;

class ModulePhysX : public Module
{
	friend class PanelPhysics;
	friend class ModuleObjects;
	friend class ComponentPhysics;
	friend class ComponentCollider;
	friend class ComponentBoxCollider;
	friend class ComponentSphereCollider;
	friend class ComponentCapsuleCollider;
	friend class ComponentMeshCollider;
	friend class ComponentConvexHullCollider;
	friend class ComponentCharacterController;
	friend class ComponentBasePhysic;

public:

	ModulePhysX(bool start_enabled = true);
	~ModulePhysX();

	void SetGravity(float3 gravity);
	float3 GetGravity();

	//* ---------- SCENE QUERIES ------------*//

	bool Raycast(float3 origin, float3 unit_dir, float max_distance) const;
	bool Raycast(float3 origin, float3 unit_dir, float max_distance, RaycastHit& hit) const;
	const std::vector<RaycastHit> RaycastAll(float3 origin, float3 unitDir, float maxDistance) const;
	const std::vector<ComponentCollider*> OverlapSphere(float3 center, float radius) const;

private:

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

private:

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

	// Delay Libraries -----------------------------------------

	bool LoadPhysicsExplicitely();
	void UnloadPhysicsExplicitely();

public:

	CollisionLayers	layers;
	bool debug_physics = false;

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
	ControllerFilterCallback*	px_controller_filter_callback = nullptr;
	PxFoundation*				px_foundation = nullptr;
	PxPhysics*					px_physics = nullptr;
	PxCooking*					px_cooking = nullptr;
	PxDefaultCpuDispatcher*		px_dispatcher = nullptr;
	PxScene*					px_scene = nullptr;
	PxPvd*						px_pvd = nullptr;

	PxControllerManager*		controllers_manager = nullptr;

};