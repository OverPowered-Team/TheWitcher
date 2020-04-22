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
	px_controller_filter_callback = new ControllerFilterCallback();

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
		px_scene->simulate(fixed_dt); // TODO, fixed time step / substeps
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
	PX_RELEASE(px_scene);
	PX_RELEASE(px_dispatcher);
	PX_RELEASE(px_physics);
	delete px_simulation_callback;
	px_simulation_callback = nullptr;
	delete px_controller_filter_callback;
	px_controller_filter_callback = nullptr;

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

void ModulePhysX::SetGravity(float3 gravity)
{
}

float3 ModulePhysX::GetGravity()
{
	return float3();
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
	FreeLibrary(physx_lib);
	FreeLibrary(common_lib);
	FreeLibrary(foundation_lib);
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

void ModulePhysX::DebugDrawConvex(const float4x4& transform, const float3& scale,const PxConvexMesh* mesh, const float3& color) const
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

// * --------------------- SCENE QUERIES ----------------------- * //

bool ModulePhysX::Raycast(float3 origin, float3 unitDir, float maxDistance, PxRaycastBuffer& hit) const
{
	PxVec3 _origin = F3_TO_PXVEC3(origin);
	PxVec3 _unitDir = F3_TO_PXVEC3(unitDir);

	//PxQueryFilterData _filterData = PxQueryFilterData(PxQueryFlag::eANY_HIT);
	//return px_scene->raycast(_origin, _unitDir, maxDistance, hit, PxHitFlag::eDEFAULT, _filterData);  // TODO: implement filtering (layermask | queryTriggerInteraction)

	return px_scene->raycast(_origin, _unitDir, maxDistance, hit);  // TODO: implement filtering (layermask | queryTriggerInteraction)
}

const std::vector<PxRaycastHit> ModulePhysX::RaycastAll(float3 origin, float3 unitDir, float maxDistance) const
{
	PxVec3 _origin = F3_TO_PXVEC3(origin);
	PxVec3 _unitDir = F3_TO_PXVEC3(unitDir);

	const PxU32 bufferSize = 256;
	PxRaycastHit hitBuffer[bufferSize];
	PxRaycastBuffer hit(hitBuffer, bufferSize);
	std::vector<PxRaycastHit> return_hits;;
	if (px_scene->raycast(_origin, _unitDir, maxDistance, hit))
	{
		for (uint i = 0; i < hit.getNbAnyHits(); ++i)
		{
			return_hits.push_back(hit.getAnyHit(i));
		}
	}

	return return_hits;
}


const std::vector<ComponentCollider*> ModulePhysX::OverlapSphere(float3 center, float radius) const
{
	PxVec3 _center = F3_TO_PXVEC3(center);

	PxSphereGeometry sphere = PxSphereGeometry(radius);
	PxTransform shapePose = PxTransform(F3_TO_PXVEC3(center));

	std::vector<ComponentCollider*> colliders;

	const PxU32 bufferSize = 256;
	PxOverlapHit hitBuffer[bufferSize];
	PxOverlapBuffer hit(hitBuffer, bufferSize);
	// filter data any  (without user buffer)
	//PxQueryFilterData filterData = PxQueryFilterData(PxQueryFlag::eNO_BLOCK);
	if (px_scene->overlap(PxSphereGeometry(radius), shapePose, hit)) // TODO: implement filtering (layermask | queryTriggerInteraction)
	{
		for (uint i = 0; i < hit.getNbAnyHits(); ++i) // TODO: change this to get only touched shapes by explicit filtering | any if no filtering
		{
			ComponentCollider* col = (ComponentCollider*)hit.getAnyHit(i).shape->userData; // user data must be set to component colliders in any shape that physx create
			if (col)
				colliders.push_back(col);
		}
	}

	return colliders;
}

// * ----------------------------------------------------------- * //

// character controller ---------------------------------------------------------------

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

// -------------------------------------------------------------------------------------