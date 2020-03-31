#ifndef _MODULE_PHYSICS_H__
#define _MODULE_PHYSICS_H__

#include "Module.h"
#include "Bullet/include/btBulletDynamicsCommon.h"
#include "Bullet/include/LinearMath/btVector3.h"
#include <list>
#include <vector>

enum class Space
{
	Global,
	Local
};

enum class ForceMode : uint
{
	FORCE ,
	VELOCITY,
	IMPULSE,
	MAX
};


class ComponentCollider;
class DebugRenderer;
class btGhostObject;

class ModulePhysics : public Module
{
	friend class Application;
	friend class ComponentCharacterController;
	friend class ModuleObjects;
	friend class ComponentCollider;
	friend class ComponentRigidBody;
	friend class ComponentPointConstraint;
	friend class ComponentConvexHullCollider;
	friend class PanelPhysics;
	friend class MyOwnFilterCallback;

public:
	ModulePhysics( bool start_enabled = true);

	~ModulePhysics();

	void SetGravity(const float3 gravity);
	const float3 GetGravity();

	std::vector<ComponentCollider*> RayCastAll(math::Ray ray);
	ComponentCollider* RayCastClosest(math::Ray ray);
	std::vector<ComponentCollider*>  SphereCast(float3 position, float radius);

private:

	void LoadConfig(JSONfilepack*& config);
	void SaveConfig(JSONfilepack*& config);

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	static bool CanCollide(int layer_0, int layer_1);

	void DrawCollider(ComponentCollider* collider);
	void DrawConvexCollider(ComponentCollider* collider);
	void DrawConstraint(btTypedConstraint* constraint);
	void DrawCharacterController(ComponentCharacterController* controller);
	void DrawWorld();

	void AddBody(btRigidBody* body);
	void RemoveBody(btRigidBody* body);

	void AddDetector(btGhostObject* detector);
	void RemoveDetector(btGhostObject* detector);

	void AddAction(btActionInterface* action);
	void RemoveAction(btActionInterface* action);

	void AddConstraint(btTypedConstraint* constraint, bool bodiesCollision = true);
	void RemoveConstraint(btTypedConstraint* constraint);

	void AddVehicle(btRaycastVehicle* vehicle);
	void RemoveVehicle(btRaycastVehicle* vehicle);

public:

	btDefaultVehicleRaycaster* vehicle_raycaster = nullptr;

private:

	std::vector<std::string> layers;
	bool**layers_table = nullptr;

	bool debug_physics = false;
	float3 gravity = float3(0.f, -9.8f, 0.f);
	DebugRenderer* debug_renderer = nullptr;
	btDefaultCollisionConfiguration* collision_config = nullptr;
	btCollisionDispatcher* dispatcher = nullptr;
	btBroadphaseInterface* broad_phase = nullptr;
	btSequentialImpulseConstraintSolver* solver = nullptr;
	btDiscreteDynamicsWorld* world = nullptr;
	std::list<btTypedConstraint*> constraints;
};

struct MyOwnFilterCallback : public btOverlapFilterCallback
{
	// return true when pairs need collision
	virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const;
};

struct CastResult : public btCollisionWorld::ContactResultCallback
{
	std::vector<ComponentCollider*> hit_colliders;
	btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1);
};

class DebugRenderer : public btIDebugDraw
{
public:
	DebugRenderer()
	{}

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);
	void reportErrorWarning(const char* warningString);

	void draw3dText(const btVector3& location, const char* textString);

	void setDebugMode(int debugMode);

	int getDebugMode() const;

	DebugDrawModes mode;
};

btVector3 ToBtVector3(const float3& vec);

btQuaternion ToBtQuaternion(const Quat& quat);

btTransform ToBtTransform(const btVector3& pos, const btQuaternion& quat);

btTransform ToBtTransform(const float3& pos, const Quat& quat);

btTransform ToBtTransform(const float3& pos, const float3x3& rotation);

#endif // !_MODULE_PHYSICS_H__

