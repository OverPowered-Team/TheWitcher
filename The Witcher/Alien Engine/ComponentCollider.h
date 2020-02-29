#pragma once

#include "Component.h"
#include "MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"
#include "BulletCollision\CollisionDispatch\btGhostObject.h"

class GameObject;
class ModulePhysics;
class ComponentMesh;
class ComponentRigidBody;
class ComponentVehicle;
class ComponentTransform;

class __declspec(dllexport) ComponentCollider : public Component
{
	friend class GameObject;
	friend class ModuleObjects;
	friend class ModulePhysics;
	friend class ReturnZ;
	friend class CompZ;
	friend class ComponentRigidBody;

public:

	ComponentCollider(GameObject* go);
	virtual ~ComponentCollider();

	// Colliders values

	virtual void SetCenter(float3 center);
	float3 GetCenter() { return center; }
	void SetIsTrigger(bool is_trigger);
	bool GetIsTrigger() { return is_trigger; }
	void SetBouncing(const float bouncing);
	float GetBouncing() { return bouncing; }
	void SetFriction(const float v);
	float GetFriction() { return friction; }
	void SetAngularFriction(const float v);
	float GetAngularFriction() { return angular_friction; }

protected:

	float3 GetWorldCenter();

	void Init();
	void Update();
	void DrawScene();
	bool DrawInspector();

	virtual void DrawSpecificInspector() {}

	virtual void Reset();
	virtual void Clone(Component* clone) {}
	virtual void SetComponent(Component* component) {}
	virtual void SaveComponent(JSONArraypack* to_save);
	virtual void LoadComponent(JSONArraypack* to_load);

	void CreateShape();	// Create shape
	virtual bool WrapMesh() { return false; }
	virtual void UpdateShape() {} 	// Adjust shape to scale and other factors

protected:

	std::string name = "Collider";
	ComponentTransform* transform = nullptr;
	ComponentRigidBody* rb = nullptr;

	float3 center = float3::zero();
	float3 final_center = float3::zero();

	bool is_trigger = false;
	float bouncing = 0.f;
	float friction = 0.f;
	float angular_friction = 0.f;

	// Collider shape used in collision simulation
	btCollisionShape* shape = nullptr;
	// Used when GameObject has notrigid body in run time
	btRigidBody* aux_body = nullptr;
};
