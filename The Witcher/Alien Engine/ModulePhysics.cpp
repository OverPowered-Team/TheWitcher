#include "Globals.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "Bullet/include/BulletCollision/CollisionShapes/btShapeHull.h"
#include "ComponentCollider.h"
#include "ComponentRigidBody.h"
#include "ComponentCharacterController.h"
#include "ComponentScript.h"
#include "ModuleRenderer3D.h"
#include "Time.h"
#include "Alien.h"
#include "Optick/include/optick.h"
#include "mmgr/mmgr.h"

ModulePhysics::ModulePhysics(bool start_enabled) : Module(start_enabled)
{
	gravity = float3(0.f, -9.8f, 0.f);
}

ModulePhysics::~ModulePhysics()
{
	LOG_ENGINE("Destroying 3D Physics simulation");

	for (int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = world->getCollisionObjectArray()[i];
		world->removeCollisionObject(obj);
	}

	delete vehicle_raycaster;
	delete world;
	delete debug_renderer;
	delete solver;
	delete broad_phase;
	delete dispatcher;
	delete collision_config;
}

void ModulePhysics::SetGravity(const float3 gravity)
{
	this->gravity = gravity;
	if (world)
		world->setGravity(ToBtVector3(gravity));
}

const float3 ModulePhysics::GetGravity()
{
	return gravity;
}

void ModulePhysics::SaveConfig(JSONfilepack*& config)
{
	config->SetFloat3("Gravity", gravity);
}

void ModulePhysics::LoadConfig(JSONfilepack*& config)
{
	SetGravity(config->GetFloat3("Gravity"));
}

bool ModulePhysics::Init()
{
	OPTICK_EVENT();
	LOG_ENGINE("Creating 3D Physics simulation");

	bool ret = true;

	debug_renderer = new DebugRenderer();
	collision_config = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collision_config);
	broad_phase = new btDbvtBroadphase();
	broad_phase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	solver = new btSequentialImpulseConstraintSolver();

	world = new btDiscreteDynamicsWorld(dispatcher, broad_phase, solver, collision_config);

	gravity = float3(0.f, -9.8f, 0.f);
	world->setGravity(ToBtVector3(gravity));
	world->setDebugDrawer(debug_renderer);
	vehicle_raycaster = new btDefaultVehicleRaycaster(world);


	btOverlapFilterCallback* filterCallback = new MyOwnFilterCallback();
	world->getPairCache()->setOverlapFilterCallback(filterCallback);

	return ret;
}

bool ModulePhysics::Start()
{
	LOG_ENGINE("Creating Physics environment");

	layers.push_back("Default");
	layers.push_back("Player");
	layers.push_back("Arrow");
	layers.push_back("Wall");
	layers.push_back("Ground");


	int size = layers.size();
	layers_table = new bool* [size];
	for (int i = 0; i < size; i++)
		layers_table[i] = new bool[size];

	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			layers_table[i][j] = true;

	return true;
}

// ---------------------------------------------------------
update_status ModulePhysics::PreUpdate(float dt)
{
	static bool first_frame_playing = true;
	OPTICK_EVENT();

	world->stepSimulation(Time::GetDT(), 20);

	return UPDATE_CONTINUE;
}

update_status ModulePhysics::PostUpdate(float dt)
{
	//if (App->input->GetKey(SDL_Scancode::SDL_SCANCODE_SPACE) == KEY_DOWN)
	//{
	//	SphereCast(float3(0.f, 0.f, 0.f), 0.5f);
	//	RayCastAll(Ray(float3(0.f, 0.f, 0.f), float3(0.f, 10.f, 0.f)));
	//	RayCastClosest(Ray(float3(0.f, 0.f, 0.f), float3(0.f, 10.f, 0.f)));

	//}
	return UPDATE_CONTINUE;
}

bool ModulePhysics::CleanUp()
{

	return true;
}

bool ModulePhysics::CanCollide(int layer_0, int layer_1)
{
	return (layer_0 < layer_1)  ? App->physics->layers_table[layer_0][layer_1] : App->physics->layers_table[layer_1][layer_0];
}

std::vector<ComponentCollider*> ModulePhysics::RayCastAll(math::Ray ray)
{
	std::vector<ComponentCollider*> return_vector;
	btCollisionWorld::AllHitsRayResultCallback results(ToBtVector3(ray.pos), ToBtVector3(ray.dir));
	world->rayTest(ToBtVector3(ray.pos), ToBtVector3(ray.dir), results);
	int size = results.m_collisionObjects.size();

	for (int i = 0; i < size; ++i)
	{
		const btCollisionObject* obj = results.m_collisionObjects.at(i);
		ComponentCollider* collider = (ComponentCollider*)obj->getUserPointer();

		if (collider != nullptr)
		{
			return_vector.push_back(collider);
		}
	}

	return return_vector;
}

ComponentCollider* ModulePhysics::RayCastClosest(math::Ray ray)
{
	btCollisionWorld::ClosestRayResultCallback results(ToBtVector3(ray.pos), ToBtVector3(ray.dir));
	world->rayTest(ToBtVector3(ray.pos), ToBtVector3(ray.dir), results);
	const btCollisionObject* obj = results.m_collisionObject;

	if (obj != nullptr)
	{
		ComponentCollider* collider = (ComponentCollider*)obj->getUserPointer();

		if (collider != nullptr)
		{
			return collider;
		}
	}

	return nullptr;
}

std::vector<ComponentCollider*> ModulePhysics::SphereCast(float3 position, float radius)
{
	btSphereShape* shape = new btSphereShape(radius);
	btGhostObject* ghost = new btGhostObject();
	btTransform transform(btQuaternion::getIdentity(), ToBtVector3(position));
	CastResult result;

	ghost->setCollisionShape(shape);
	ghost->setWorldTransform(transform);
	world->contactTest(ghost, result);

	delete ghost;
	delete shape;

	return result.hit_colliders;
}


void ModulePhysics::DrawCollider(ComponentCollider* collider)
{
	debug_renderer->setDebugMode(btIDebugDraw::DBG_FastWireframe);
	ModuleRenderer3D::BeginDebugDraw(float4(0.f, 1.f, 0.f, 1.f));

	world->debugDrawObject(
		((collider->rigid_body) ? collider->rigid_body->body : collider->aux_body)->getWorldTransform()
		, collider->shape, btVector3(0.f, 1.f, 0.f));

	ModuleRenderer3D::EndDebugDraw();
}

void ModulePhysics::DrawConvexCollider(ComponentCollider* collider)
{
	debug_renderer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	ModuleRenderer3D::BeginDebugDraw(float4(0.f, 1.f, 0.f, 1.f));

	btTransform worldTransform = ((collider->rigid_body) ? collider->rigid_body->body : collider->aux_body)->getWorldTransform();
	btShapeHull* hull = static_cast<btShapeHull*>(collider->shape->getUserPointer());

	if (hull == nullptr) return;

	int num_indices = hull->numIndices();
	btVector3 localScale = collider->shape->getLocalScaling();
	for (int i = 0; i < num_indices; i += 3)
	{
		btVector3 v0 = worldTransform * (hull->getVertexPointer()[hull->getIndexPointer()[i]] * localScale);
		btVector3 v1 = worldTransform * (hull->getVertexPointer()[hull->getIndexPointer()[i + 1]] * localScale);
		btVector3 v2 = worldTransform * (hull->getVertexPointer()[hull->getIndexPointer()[i + 2]] * localScale);

		btVector3 color = btVector3(0.f, 1.f, 0.f);
		debug_renderer->drawLine(v0, v1, color);
		debug_renderer->drawLine(v1, v2, color);
		debug_renderer->drawLine(v0, v2, color);
	}

	ModuleRenderer3D::EndDebugDraw();
}

void ModulePhysics::DrawConstraint(btTypedConstraint* constraint)
{
	debug_renderer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	ModuleRenderer3D::BeginDebugDraw(float4(1.f, 1.f, 0.f, 1.f));
	world->debugDrawConstraint(constraint);
	ModuleRenderer3D::EndDebugDraw();
}

void ModulePhysics::DrawCharacterController(ComponentCharacterController* controller)
{
	debug_renderer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	ModuleRenderer3D::BeginDebugDraw(float4(1.f, 1.f, 0.f, 1.f));
	world->debugDrawObject(controller->body->getWorldTransform(), controller->shape, btVector3(0.f, 1.f, 0.f));
	ModuleRenderer3D::EndDebugDraw();
}

void ModulePhysics::DrawWorld()
{
	debug_renderer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);

	ModuleRenderer3D::BeginDebugDraw(float4(0.f, 1.f, 0.f, 1.f));
	world->debugDrawWorld();
	ModuleRenderer3D::EndDebugDraw();

}

void ModulePhysics::AddBody(btRigidBody* body)
{
	world->addRigidBody(body);
}

void ModulePhysics::RemoveBody(btRigidBody* body)
{
	world->removeRigidBody(body);
}

void ModulePhysics::AddDetector(btGhostObject* detector)
{
	world->addCollisionObject(detector, btBroadphaseProxy::SensorTrigger, btBroadphaseProxy::SensorTrigger);
}

void ModulePhysics::RemoveDetector(btGhostObject* detector)
{
	world->removeCollisionObject(detector);
}

void ModulePhysics::AddAction(btActionInterface* action)
{
	world->addAction(action);
}

void ModulePhysics::RemoveAction(btActionInterface* action)
{
	world->removeAction(action);
}

void ModulePhysics::AddConstraint(btTypedConstraint* constraint, bool disableBodiesCollision)
{
	world->addConstraint(constraint, disableBodiesCollision);
}

void ModulePhysics::RemoveConstraint(btTypedConstraint* constraint)
{
	world->removeConstraint(constraint);
}

void ModulePhysics::AddVehicle(btRaycastVehicle* vehicle)
{
	world->addVehicle(vehicle);
}

void ModulePhysics::RemoveVehicle(btRaycastVehicle* vehicle)
{
	world->removeVehicle(vehicle);
}

void DebugRenderer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	glColor3fv(&color[0]);
	glBegin(GL_LINES);
	glVertex3fv(&from[0]);
	glVertex3fv(&to[0]);
	glEnd();
}

void DebugRenderer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	//point.transform.translate(PointOnB.getX(), PointOnB.getY(), PointOnB.getZ());
	//point.color.Set(color.getX(), color.getY(), color.getZ());
	//point.Render();
}

void DebugRenderer::reportErrorWarning(const char* warningString)
{
	LOG_ENGINE("Bullet warning: %s", warningString);
}

void DebugRenderer::draw3dText(const btVector3& location, const char* textString)
{
	LOG_ENGINE("Bullet draw text: %s", textString);
}

void DebugRenderer::setDebugMode(int debugMode)
{
	mode = (DebugDrawModes)debugMode;
}

int	DebugRenderer::getDebugMode() const
{
	return mode;
}

btVector3 ToBtVector3(const float3& vec)
{
	return btVector3(vec.x, vec.y, vec.z);
}

btQuaternion ToBtQuaternion(const Quat& quat)
{
	return btQuaternion(quat.x, quat.y, quat.z, quat.w);
}

btTransform ToBtTransform(const btVector3& pos, const  btQuaternion& quat)
{
	btTransform trans;
	trans.setOrigin(pos);
	trans.setRotation(quat);
	return trans;
}

btTransform ToBtTransform(const float3& pos, const Quat& quat)
{
	btTransform trans;
	trans.setOrigin(ToBtVector3(pos));
	trans.setRotation(ToBtQuaternion(quat));
	return trans;
}

btTransform ToBtTransform(const float3& pos, const float3x3& rotation)
{
	const float* rot = rotation.ptr();
	btMatrix3x3 mat(rot[0], rot[1], rot[2], rot[3], rot[4], rot[5], rot[6], rot[7], rot[8]);
	btTransform trans(mat, ToBtVector3(pos));
	return trans;
}

btScalar CastResult::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
{
	const btCollisionObject* obj = colObj1Wrap->m_collisionObject;

	if (obj != nullptr)
	{
		ComponentCollider* collider = (ComponentCollider*)obj->getUserPointer();

		if (collider != nullptr)
		{
			hit_colliders.push_back(collider);
		}
	}

	return 1;
}

bool MyOwnFilterCallback::needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const
{
	btCollisionObject* obj_0 = (btCollisionObject*)proxy0->m_clientObject;
	btCollisionObject* obj_1 = (btCollisionObject*)proxy1->m_clientObject;
	ComponentCollider* coll_0 = (ComponentCollider * )obj_0->getUserPointer();
	ComponentCollider* coll_1 = (ComponentCollider * )obj_1->getUserPointer();

	if (coll_0 == nullptr || coll_1 == nullptr) 
		return true;
	
	//add some additional logic here that modified 'collides'
	if (ModulePhysics::CanCollide(coll_0->layer, coll_1->layer))
	{
		return true;
	}
	else
	{
		return false;
	}
}