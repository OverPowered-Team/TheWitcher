#ifndef _MODULE_PHYSICS_H__
#define _MODULE_PHYSICS_H__

#include "Module.h"
#include "Bullet/include/btBulletDynamicsCommon.h"
#include "Bullet/include/LinearMath/btVector3.h"
#include <list>

// Recommended scale is 1.0f == 1 meter, no less than 0.2 objects
#define GRAVITY btVector3(0.0f, -9.8f, 0.0f) 

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
	friend class ComponentCharacterController;
	friend class ModuleObjects;
	friend class ComponentCollider;
	friend class ComponentConvexHullCollider;
	friend class PanelPhysics;

public:
	ModulePhysics( bool start_enabled = true);

	~ModulePhysics();

	void LoadConfig(JSONfilepack*& config);

	void SaveConfig(JSONfilepack*& config);

	bool Init();

	bool Start();

	update_status PreUpdate(float dt);

	update_status PostUpdate(float dt);

	bool CleanUp();

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

	bool debug_physics = false;
	DebugRenderer* debug_renderer = nullptr;
	btDefaultCollisionConfiguration* collision_config = nullptr;
	btCollisionDispatcher* dispatcher = nullptr;
	btBroadphaseInterface* broad_phase = nullptr;
	btSequentialImpulseConstraintSolver* solver = nullptr;
	btDiscreteDynamicsWorld* world = nullptr;
	std::list<btTypedConstraint*> constraints;
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

#endif // !_MODULE_PHYSICS_H__

