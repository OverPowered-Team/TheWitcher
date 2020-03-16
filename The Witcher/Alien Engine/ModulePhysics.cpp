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

ModulePhysics::ModulePhysics(bool start_enabled) : Module(start_enabled)
{
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

void ModulePhysics::LoadConfig(JSONfilepack*& config)
{

}
void ModulePhysics::SaveConfig(JSONfilepack*& config)
{

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

	world->setGravity(GRAVITY);
	world->setDebugDrawer(debug_renderer);
	vehicle_raycaster = new btDefaultVehicleRaycaster(world);

	return ret;
}

bool ModulePhysics::Start()
{
	LOG_ENGINE("Creating Physics environment");

	return true;
}

// ---------------------------------------------------------
update_status ModulePhysics::PreUpdate(float dt)
{
	static bool first_frame_playing = true;
	OPTICK_EVENT();

	if (Time::IsInGameState())
	{
		if (first_frame_playing == false)
		{
			world->stepSimulation(Time::GetDT(), 20);
		}
		
		first_frame_playing = false;
	}
	else
	{
		first_frame_playing = true;
	}

	return UPDATE_CONTINUE;
}

update_status ModulePhysics::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModulePhysics::CleanUp()
{
	return true;
}

void ModulePhysics::DrawCollider(ComponentCollider* collider)
{
	debug_renderer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
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
	debug_renderer->setDebugMode(btIDebugDraw::DBG_DrawConstraints);
	ModuleRenderer3D::BeginDebugDraw(float4(1.f, 1.f, 0.f, 1.f));
	world->debugDrawConstraint(constraint);
	ModuleRenderer3D::EndDebugDraw();
}

void ModulePhysics::DrawCharacterController(ComponentCharacterController* controller)
{
	debug_renderer->setDebugMode(btIDebugDraw::DBG_FastWireframe);
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
	glColor3f(color[0], color[1], color[2]);
	glBegin(GL_LINES);
	glVertex3f(from[0], from[1], from[2]);
	glVertex3f(to[0], to[1], to[2]);
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
	return btTransform(quat, pos);
}

btTransform ToBtTransform(const float3& pos, const Quat& quat)
{
	return btTransform(ToBtQuaternion(quat), ToBtVector3(pos));
}
