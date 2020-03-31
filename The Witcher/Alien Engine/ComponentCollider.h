#pragma once

#include <map>
#include <list>
#include "Component.h"
#include "MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"
#include "BulletCollision\CollisionDispatch\btGhostObject.h"
#include "Event.h"

class GameObject;
class ModulePhysics;
class ComponentMesh;
class ComponentRigidBody;
class ComponentVehicle;
class ComponentTransform;
class Alien;

class __declspec(dllexport) ComponentCollider : public Component
{
	friend class GameObject;
	friend class ReturnZ;
	friend class CompZ;

	friend class ModuleObjects;
	friend class ModulePhysics;
	friend class ComponentCharacterController;
	friend class ComponentRigidBody;
	friend class MyDispatcher;
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

	void AddToWorld();
	void RemoveFromWorld();

	void Init();
	void Update();
	void OnEnable();
	void OnDisable();

	bool DrawInspector();
	void HandleAlienEvent(const AlienEvent& e);

	virtual void DrawSpecificInspector() {}
	virtual void DrawScene();
	virtual void Reset();
	virtual void Clone(Component* clone) {}
	virtual void SetComponent(Component* component) {}
	virtual void SaveComponent(JSONArraypack* to_save);
	virtual void LoadComponent(JSONArraypack* to_load);

	virtual void CreateDefaultShape() {};
	virtual void UpdateShape() {} 	// Adjust shape to scale and other factors
	virtual void SetScale(float3 scale);

protected:

	std::string name = "Collider";
	ComponentTransform* transform = nullptr;
	ComponentRigidBody* rigid_body = nullptr;

	float3 center = float3::zero();
	float3 final_center = float3::zero();
	float3 last_scale = float3::zero();
	bool is_trigger = false;
	float bouncing = 0.f;
	float friction = 0.f;
	float angular_friction = 0.f;

	// Collider shape used in collision simulation
	btCollisionShape* shape = nullptr;
	// Used when GameObject has notrigid body in run time
	btRigidBody* aux_body = nullptr;
	// Detection body 
	btPairCachingGhostObject* detector = nullptr;

	// Alien Script 
	std::list<ComponentScript*> alien_scripts;
	// Collisions
	std::map<ComponentCollider*, bool> collisions;

	bool first_frame = false;
	bool internal_collider = false;
	bool added_to_world = false;
	int  layer = 0;
};
