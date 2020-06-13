#include "Globals.h"
#include "Application.h"
#include "ModulePhysX.h"
#include "ModuleRenderer3D.h"

#include "Component.h"
#include "ComponentPhysics.h"
#include "ComponentCollider.h"
#include "ComponentRigidBody.h"

#include "PxPhysicsAPI.h"
#include "Time.h"
#include "UtilitiesPhysX.h"
#include "Optick/include/optick.h"
#include "CollisionLayers.h"



ModulePhysX::ModulePhysX(bool start_enabled) : Module(start_enabled)
{

}

ModulePhysX::~ModulePhysX()
{
}

bool ModulePhysX::Init()
{
	OPTICK_EVENT();
	LOG_ENGINE("Creating 3D PhysX world");
	bool ret = true;

	// load the explictely named dlls
	const bool isLoaded = LoadPhysicsExplicitely(); // load debug or release dlls
	if (!isLoaded)
		return false;

	px_simulation_callback = new SimulationEventCallback();
	px_controller_filter = new ControllerFilterCallback();
	px_query_filter = new QueryFilterCallback();

	// TODO: make init blindings if any stage goes wrong

	px_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, px_allocator, px_error_callback);

	// set PhysX and PhysXCommon delay load hook, this must be done before the create physics is called, before ----
	// the PhysXFoundation, PhysXCommon delay load happens.
	PxSetPhysXDelayLoadHook(&delayLoadHook);
	PxSetPhysXCommonDelayLoadHook(&delayLoadHook);
	PxSetPhysXCookingDelayLoadHook(&delayLoadHook);

	// set PhysXGpu load hook
	PxSetPhysXGpuLoadHook(&gpuLoadHook);
	// --------------------------------------------------------------------------------------------------------------

	px_pvd = PxCreatePvd(*px_foundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	px_pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	px_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *px_foundation, PxTolerancesScale(), true, px_pvd);
	px_cooking = PxCreateCooking(PX_PHYSICS_VERSION, *px_foundation, PxCookingParams(PxTolerancesScale()));

	if (!px_cooking)
		LOG_ENGINE("PxCreateCooking failed!");

	PxSceneDesc sceneDesc(px_physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	px_dispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = px_dispatcher;
	sceneDesc.filterShader = FilterShader;
	sceneDesc.kineKineFilteringMode = PxPairFilteringMode::eKEEP;
	sceneDesc.staticKineFilteringMode = PxPairFilteringMode::eKEEP;
	px_scene = px_physics->createScene(sceneDesc);
	px_scene->setSimulationEventCallback(px_simulation_callback);
	px_scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	px_scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
	//px_scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_AABBS, 1.0f);


	PxPvdSceneClient* pvdClient = px_scene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	//PxMaterial* m = px_physics->createMaterial(0.5f, 0.5f, 0.6f);

	//PxShape* s = px_physics->createShape(PxPlaneGeometry(), *m);
	//PxRigidDynamic* a = px_physics->createRigidDynamic(PxTransform(QUAT_TO_PXQUAT(Quat::FromEulerXYZ(0,0, 90 * DEGTORAD))));
	//a->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	//a->attachShape(*s);
	//PxFilterData data(0, 3, 0, 0);
	//s->setQueryFilterData(data);
	//s->setSimulationFilterData(data);
	//px_scene->addActor(*a);

	// create characters controller manager
	if (px_scene)
		controllers_manager = PxCreateControllerManager(*px_scene);


	// Filtering --------------------------------------

	layers.LoadLayers();

	return ret;
}

bool ModulePhysX::Start()
{
	LOG_ENGINE("Creating Physics environment");


	return true;
}

// ---------------------------------------------------------
update_status ModulePhysX::PreUpdate(float dt)
{
	static bool first_frame_playing = true;
	OPTICK_EVENT();

	float fixed_dt = 1.0f / 60.0f;
	float game_dt = Time::GetDT();

	if (Time::IsPlaying())
	{
		//accumulator += game_dt;
		//if (accumulator < fixed_dt)
		//	return UPDATE_CONTINUE;
		//accumulator -= fixed_dt;
		px_scene->simulate(game_dt); // TODO, fixed time step / substeps
		px_scene->fetchResults(true);
	}

	return UPDATE_CONTINUE;
}

update_status ModulePhysX::PostUpdate(float dt)
{

	return UPDATE_CONTINUE;
}

bool ModulePhysX::CleanUp()
{
	
	PX_RELEASE(controllers_manager);
	PX_RELEASE(px_scene);
	PX_RELEASE(px_dispatcher);
	PX_RELEASE(px_cooking);
	PX_RELEASE(px_physics);
	
	delete px_simulation_callback;
	px_simulation_callback = nullptr;
	delete px_controller_filter;
	px_controller_filter = nullptr;
	delete px_query_filter;
	px_query_filter = nullptr;

	if (px_pvd)
	{
		PxPvdTransport* transport = px_pvd->getTransport();
		px_pvd->release();	px_pvd = NULL;
		PX_RELEASE(transport);
	}

	PX_RELEASE(px_foundation);

	UnloadPhysicsExplicitely();
	layers.SaveLayers();

	return true;
}

void ModulePhysX::SetGravity(float3 _gravity)
{
	gravity = _gravity;
	px_scene->setGravity(F3_TO_PXVEC3(_gravity));
}

float3 ModulePhysX::GetGravity()
{
	return gravity;
}

bool ModulePhysX::LoadPhysicsExplicitely()
{
	// load the dlls

	foundation_lib = LoadLibraryA(foundation_lib_path);
	if (!foundation_lib)
		return false;

	common_lib = LoadLibraryA(common_lib_path);
	if (!common_lib)
	{
		FreeLibrary(foundation_lib);
		return false;
	}

	physx_lib = LoadLibraryA(physx_lib_path);
	if (!physx_lib)
	{
		FreeLibrary(foundation_lib);
		FreeLibrary(common_lib);
		return false;
	}

	cooking_lib = LoadLibraryA(cooking_lib_path);
	if (!cooking_lib)
	{
		FreeLibrary(foundation_lib);
		FreeLibrary(common_lib);
		FreeLibrary(physx_lib);
		return false;
	}
	return true;
}

void ModulePhysX::UnloadPhysicsExplicitely()
{
	FreeLibrary(common_lib);
	FreeLibrary(foundation_lib);
	FreeLibrary(physx_lib);
}

void ModulePhysX::DrawCollider(ComponentCollider* collider)
{
	float4x4 trans(PXTRANS_TO_F4X4(collider->physics->actor->getGlobalPose() * collider->shape->getLocalPose()));
	float3 color = float3(0.f, 1.f, 0.f);

	switch (collider->type)
	{
	case ComponentType::BOX_COLLIDER: {
		PxBoxGeometry geo;
		collider->shape->getBoxGeometry(geo);
		App->renderer3D->DebugDrawBox(trans, PXVEC3_TO_F3(geo.halfExtents), color);
		break; }
	case ComponentType::SPHERE_COLLIDER: {
		PxSphereGeometry geo;
		collider->shape->getSphereGeometry(geo);
		App->renderer3D->DebugDrawSphere(trans, geo.radius, color);
		break; }
	case ComponentType::CAPSULE_COLLIDER: {
		PxCapsuleGeometry geo;
		collider->shape->getCapsuleGeometry(geo);
		App->renderer3D->DebugDrawCapsule(trans, geo.radius, geo.halfHeight, color);
		break; }
	case ComponentType::CONVEX_HULL_COLLIDER: {
		PxConvexMeshGeometry geo;
		collider->shape->getConvexMeshGeometry(geo);
		DebugDrawConvex(trans, PXVEC3_TO_F3(geo.scale.scale), geo.convexMesh, color);
		break; }

	default:
		break;
	}
}

void ModulePhysX::DebugDrawConvex(const float4x4& transform, const float3& scale, const PxConvexMesh* mesh, const float3& color) const
{
	glPushMatrix();
	float4x4 trans = (transform * trans.Scale(scale)).Transposed();
	glMultMatrixf(trans.ptr());
	ModuleRenderer3D::BeginDebugDraw(float4(color.x, color.y, color.z, 1.0f));

	const PxU32 npolys = mesh->getNbPolygons();
	const PxU8* indices = mesh->getIndexBuffer();
	const PxVec3* vertices = mesh->getVertices();

	for (uint i = 0; i < npolys; ++i) {
		PxHullPolygon poly;
		if (!mesh->getPolygonData(i, poly))
			continue;

		glBegin(GL_LINE_LOOP);
		PxU16 idx_offset = poly.mIndexBase;
		for (uint j = 0; j < poly.mNbVerts; ++j) {
			glVertex3f(vertices[indices[idx_offset + j]].x, vertices[indices[idx_offset + j]].y, vertices[indices[idx_offset + j]].z);
		}
		glEnd();
	}

	ModuleRenderer3D::EndDebugDraw();
	glPopMatrix();
}


void ModulePhysX::DrawWorld()
{
	ModuleRenderer3D::BeginDebugDraw(float4(0.f, 1.f, 0.f, 1.f));

	const PxRenderBuffer& rb = px_scene->getRenderBuffer();

	PxU32 num_lines = rb.getNbLines();
	const PxDebugLine* lines = rb.getLines();

	glBegin(GL_LINES);

	for (PxU32 i = 0; i < num_lines; i++)
	{
		glVertex3f(lines[i].pos0.x, lines[i].pos0.y, lines[i].pos0.z);
		glVertex3f(lines[i].pos1.x, lines[i].pos1.y, lines[i].pos1.z);
	}
	glEnd();

	ModuleRenderer3D::EndDebugDraw();
}

PxRigidActor* ModulePhysX::CreateBody(const float4x4& transform, bool is_dynamic)
{
	PxTransform trans;

	if (!F4X4_TO_PXTRANS(transform, trans))
	{
		return nullptr;
	}

	PxRigidActor* return_body = nullptr;
	return_body = (is_dynamic)
		? (PxRigidActor*)px_physics->createRigidDynamic(trans)
		: (PxRigidActor*)px_physics->createRigidStatic(trans);

	px_scene->addActor(*return_body);
	return return_body;
}

void ModulePhysX::RemoveBody(PxRigidActor* body)
{
	px_scene->removeActor(*body, true);
	body->release();
}

PxShape* ModulePhysX::CreateShape(const PxGeometry& geometry, const PxMaterial& material)
{
	return px_physics->createShape(geometry, material);
}

PxMaterial* ModulePhysX::CreateMaterial(float staticFriction, float dynamicFriction, float restitution) const
{
	return px_physics->createMaterial(staticFriction, dynamicFriction, restitution);
}

// Queries ------------------------------------------------------------

// ------------------ Generals ------------------------

PxHitFlags ModulePhysX::GetHitFlags()
{
	PxHitFlags flags = PxHitFlag::eDEFAULT | PxHitFlag::eUV;
	if (query_hit_backfaces) flags |= PxHitFlag::eMESH_BOTH_SIDES;
	return flags;
}
PxQueryFilterData ModulePhysX::GetFilterData( bool any_hit)
{
	PxQueryFilterData fd;
	fd.flags |= PxQueryFlag::ePREFILTER | PxQueryFlag::ePOSTFILTER;
	if (any_hit)  fd.flags |= PxQueryFlag::eANY_HIT;
	return fd;
}
void ModulePhysX::BeginQueryFilter(QueryType query_type, int layer_mask, bool generate_vector)
{
	this->query_type = query_type;
	this->layer_mask = layer_mask;
	this->multiple_hit = generate_vector;
}
void ModulePhysX::EndQueryFilter()
{
	this->query_type = QueryType::NO_TYPE;
	this->layer_mask = DEFAULT_LAYER_MASK;
	this->multiple_hit = false;
}

bool ModulePhysX::ClosestPoint(float3 point, float3& closest_point,  Collider collider)
{
	if (collider == nullptr || collider->physics == nullptr || collider->physics->actor == nullptr) return false;
	PxVec3 _closest_point(0.f, 0.f, 0.f);
	PxGeometryQuery::pointDistance(F3_TO_PXVEC3(point), collider->shape->getGeometry().any(), collider->physics->actor->getGlobalPose(), &_closest_point);
	closest_point = PXVEC3_TO_F3(_closest_point);
	return true;
}

bool ModulePhysX::ClosestPoint(float3 point, float3& closest_point, Collider collider, float3 position, Quat rotation)
{
	PxTransform px_trams;
	float4x4 trans(rotation, position);

	if (! collider || ! F4X4_TO_PXTRANS(trans, px_trams)) return false;
	PxVec3 _closest_point(0.f, 0.f, 0.f);
	PxGeometryQuery::pointDistance(F3_TO_PXVEC3(point), collider->shape->getGeometry().any(), px_trams, &_closest_point);
	closest_point = PXVEC3_TO_F3(_closest_point);

	return true;
}

bool ModulePhysX::SweepAny(PxGeometry& geometry, float4x4& trans, float3& unit_dir, float max_dist, int layer_mask)
{
	PxTransform _trans;
	if (!F4X4_TO_PXTRANS(trans, _trans)) return false;

	PxSweepBuffer sweep_buffer;
	BeginQueryFilter(QueryType::SWEEPCAST, layer_mask);
	bool ret = px_scene->sweep(geometry, _trans, F3_TO_PXVEC3(unit_dir), max_dist, sweep_buffer, GetHitFlags(), GetFilterData(true), px_query_filter);
	EndQueryFilter();

	return ret;
}
bool ModulePhysX::Sweep(PxGeometry& geometry, float4x4& trans, float3& unit_dir, float max_dist, RaycastHit& hit, int layer_mask)
{
	PxTransform _trans;
	if (!F4X4_TO_PXTRANS(trans, _trans)) return false;

	PxSweepBuffer sweep_buffer;
	BeginQueryFilter(QueryType::SWEEPCAST, layer_mask);
	bool ret = px_scene->sweep(geometry, _trans, F3_TO_PXVEC3(unit_dir), max_dist, sweep_buffer, GetHitFlags(), GetFilterData(false), px_query_filter);
	EndQueryFilter();

	if (ret) hit.SetRaycastHit(sweep_buffer.block);
	return ret;
}
void ModulePhysX::SweepAll(PxGeometry& geometry, float4x4& trans, float3& unit_dir, float max_dist, RaycastHits& hits, int layer_mask)
{
	PxTransform _trans;
	if (!F4X4_TO_PXTRANS(trans, _trans)) return;
	PxSweepHit hits_buffer[BUFFER_SIZE];
	PxSweepBuffer sweep_buffer(hits_buffer, BUFFER_SIZE);
	BeginQueryFilter(QueryType::SWEEPCAST, layer_mask, true);
	if (px_scene->sweep(geometry, _trans, F3_TO_PXVEC3(unit_dir), max_dist, sweep_buffer, GetHitFlags(), GetFilterData(false), px_query_filter)) {
		uint size = sweep_buffer.getNbAnyHits();
		for (uint i = 0; i < size; ++i) {
			RaycastHit hit;
			const PxSweepHit _hit = sweep_buffer.getAnyHit(i);
			hit.SetRaycastHit(_hit);
			hits.push_back(hit);
		}
	}
	EndQueryFilter();
}
bool ModulePhysX::Check(PxGeometry& geometry, float4x4& trans, int layer_mask)
{
	PxTransform _trans;
	if (!F4X4_TO_PXTRANS(trans, _trans)) return false;
	PxOverlapBuffer overlap_buffer;

	BeginQueryFilter(QueryType::OVERLAP, layer_mask);
	bool ret = px_scene->overlap(geometry, _trans, overlap_buffer, GetFilterData(true), px_query_filter);
	EndQueryFilter();

	return ret;
}
void ModulePhysX::Overlap(PxGeometry& geometry, float4x4& trans, Colliders& colliders, int layer_mask)
{
	PxTransform _trans;
	if (!F4X4_TO_PXTRANS(trans, _trans)) return;

	PxOverlapHit hits_buffer[BUFFER_SIZE];
	PxOverlapBuffer overlap_buffer(hits_buffer, BUFFER_SIZE);

	BeginQueryFilter(QueryType::OVERLAP, layer_mask, true);
	if (px_scene->overlap(geometry, _trans, overlap_buffer, GetFilterData(false), px_query_filter)) {
		for (uint i = 0; i < overlap_buffer.getNbAnyHits(); ++i) {
			Collider col = (Collider)overlap_buffer.getAnyHit(i).shape->userData;
			if (col) colliders.push_back(col);
		}
	}
	EndQueryFilter();
}

// ------------------ Ray ------------------------

bool ModulePhysX::Raycast(float3 origin, float3 unit_dir, float max_distance, int layer_mask)
{
	PxRaycastBuffer raycast_buffer;

	BeginQueryFilter(QueryType::RAYCAST, layer_mask);
	bool ret = px_scene->raycast(F3_TO_PXVEC3(origin), F3_TO_PXVEC3(unit_dir), max_distance, raycast_buffer, GetHitFlags(), GetFilterData(true), px_query_filter);
	EndQueryFilter();

	return ret;
}
bool ModulePhysX::Raycast(float3 origin, float3 unit_dir, float max_distance, RaycastHit& hit, int layer_mask)
{
	PxRaycastBuffer raycast_buffer;
	BeginQueryFilter(QueryType::RAYCAST, layer_mask);
	bool ret = px_scene->raycast(F3_TO_PXVEC3(origin), F3_TO_PXVEC3(unit_dir), max_distance, raycast_buffer, GetHitFlags(), GetFilterData(false), px_query_filter);
	EndQueryFilter();

	if (ret) hit.SetRaycastHit(raycast_buffer.block);
	return ret;
}
RaycastHits ModulePhysX::RaycastAll(float3 origin, float3 unit_dir, float max_distance, int layer_mask)
{
	PxRaycastHit hits_buffer[BUFFER_SIZE];
	PxRaycastBuffer raycast_buffer(hits_buffer, BUFFER_SIZE);
	RaycastHits return_hits;

	BeginQueryFilter(QueryType::RAYCAST, layer_mask, true);
	if (px_scene->raycast(F3_TO_PXVEC3(origin), F3_TO_PXVEC3(unit_dir), max_distance, raycast_buffer, GetHitFlags(), GetFilterData(false), px_query_filter)) {
		uint size = raycast_buffer.getNbAnyHits();
		for (uint i = 0; i < size; ++i) {
			RaycastHit hit;
			const PxRaycastHit _hit = raycast_buffer.getAnyHit(i);
			hit.SetRaycastHit(_hit);
			return_hits.push_back(hit);
		}
	}
	EndQueryFilter();

	return return_hits;
}

// ------------------ Box ----------------------------

bool ModulePhysX::CheckBox(float4x4& trans, float half_size, int layer_mask)
{
	return Check(PxBoxGeometry(PxVec3(half_size)), trans, layer_mask);
}
bool ModulePhysX::BoxCast(float4x4& trans, float half_size, float3 unit_dir, float max_dist, int layer_mask)
{
	return SweepAny(PxBoxGeometry(PxVec3(half_size)), trans, unit_dir, max_dist, layer_mask);
}
bool ModulePhysX::BoxCast(float4x4& trans, float half_size, float3 unit_dir, float max_dist, RaycastHit& hit, int layer_mask)
{
	return Sweep(PxBoxGeometry(PxVec3(half_size)), trans, unit_dir, max_dist, hit, layer_mask);
}
RaycastHits ModulePhysX::BoxCastAll(float4x4 trans, float half_size, float3 unit_dir, float max_dist, int layer_mask)
{
	RaycastHits hits;
	SweepAll(PxBoxGeometry(PxVec3(half_size)), trans, unit_dir, max_dist, hits, layer_mask);
	return hits;
}
Colliders ModulePhysX::OverlapBox(float4x4& trans, float half_size, int layer_mask)
{
	Colliders colliders;
	Overlap(PxBoxGeometry(PxVec3(half_size)), trans, colliders, layer_mask);
	return colliders;
}

// ------------------ Sphere -------------------------

bool ModulePhysX::CheckSphere(float3 center, float radius, int layer_mask)
{
	return Check(PxSphereGeometry(radius), float4x4(Quat::identity(), center), layer_mask);
}
bool ModulePhysX::SphereCast(float3 center, float radius, float3 unit_dir, float max_dist, int layer_mask)
{
	return SweepAny(PxSphereGeometry(radius), float4x4(Quat::identity(), center), unit_dir, max_dist, layer_mask);
}
bool ModulePhysX::SphereCast(float3 center, float radius, float3 unit_dir, float max_dist, RaycastHit& hit, int layer_mask)
{
	return Sweep(PxSphereGeometry(radius), float4x4(Quat::identity(), center), unit_dir, max_dist, hit, layer_mask);
}
RaycastHits ModulePhysX::SphereCastAll(float3 center, float radius, float3 unit_dir, float max_dist, int layer_mask)
{
	RaycastHits hits;
	SweepAll(PxSphereGeometry(radius), float4x4(Quat::identity(), center), unit_dir, max_dist, hits, layer_mask);
	return hits;
}
Colliders ModulePhysX::OverlapSphere(float3 center, float radius, int layer_mask)
{
	Colliders colliders;
	Overlap(PxSphereGeometry(radius), float4x4(Quat::identity(), center) , colliders, layer_mask);
	return colliders;
}

// ------------------ Capsule ------------------------

bool ModulePhysX::CheckCapsule(float4x4& trans, float height, float radius, int layer_mask)
{
	float4x4 final_trans = trans * Quat::RotateZ(DEGTORAD * -90.f);
	return Check(PxCapsuleGeometry(radius, height * .5f), final_trans, layer_mask);
}
bool ModulePhysX::CapsuleCast(float4x4& trans, float height, float radius, float3 unit_dir, float max_dist, int layer_mask)
{
	float4x4 final_trans = trans * Quat::RotateZ(DEGTORAD * -90.f);
	return SweepAny(PxCapsuleGeometry(radius, height * .5f), final_trans, unit_dir, max_dist, layer_mask);
}
bool ModulePhysX::CapsuleCast(float4x4& trans, float height, float radius, float3 unit_dir, float max_dist, RaycastHit& hit, int layer_mask)
{
	float4x4 final_trans = trans * Quat::RotateZ(DEGTORAD * -90.f);
	return Sweep(PxCapsuleGeometry(radius, height * .5f), final_trans, unit_dir, max_dist, hit, layer_mask);
}
RaycastHits ModulePhysX::CapsuleCastAll(float4x4 trans, float height, float radius, float3 unit_dir, float max_dist, int layer_mask)
{
	RaycastHits hits;
	float4x4 final_trans = trans * Quat::RotateZ(DEGTORAD * -90.f);
	SweepAll(PxCapsuleGeometry(radius, height * .5f), final_trans, unit_dir, max_dist, hits, layer_mask);
	return hits;
}
Colliders ModulePhysX::OverlapCapsule(float4x4& trans, float height, float radius, int layer_mask)
{
	Colliders colliders;
	float4x4 final_trans = trans * Quat::RotateZ(DEGTORAD * -90.f);
	Overlap(PxCapsuleGeometry(radius, height * .5f), final_trans, colliders, layer_mask);
	return colliders;
}

// Character controller ---------------------------------------------------------------
PxController* ModulePhysX::CreateCharacterController(PxControllerDesc& desc)
{
	return controllers_manager ? controllers_manager->createController(desc) : nullptr;
}

uint ModulePhysX::GetNbControllers() const
{
	return controllers_manager ? controllers_manager->getNbControllers() : 0;
}

PxController* ModulePhysX::GetController(uint index) const
{
	return controllers_manager ? controllers_manager->getController(index) : nullptr;
}
