#pragma once

#include <map>
#include <list>
#include "ComponentBasePhysic.h"
#include "MathGeoLib/include/Math/MathAll.h"
#include "Event.h"
#include "PxShape.h"

class GameObject;
class ComponentRigidBody;
class ComponentMesh;
class ComponentVehicle;
class ComponentTransform;
class ComponentPhysics;
class Alien;

enum class CombineMode
{
	Average =0,
	Minimum,
	Multiply,
	Maximum,
	Unknown
};


using namespace physx;

class __declspec(dllexport) ComponentCollider : public ComponentBasePhysic
{
	friend class ModuleObjects;
	friend class ModulePhysX;

	friend class ComponentCharacterController;
	friend class ComponentRigidBody;
	friend class ComponentPhysics;

	friend class SimulationEventCallback;
	friend class GameObject;
	friend class MyDispatcher;

public:

	ComponentCollider(GameObject* go);
	virtual ~ComponentCollider();

	// Colliders values

	//virtual void QueryMesh(ComponentMesh* mesh) {};
	virtual void SetCenter(const float3& value);
	float3 GetCenter() { return center; }
	virtual void SetRotation(const float3& value);
	float3 GetRotation() { return rotation; }
	void SetIsTrigger(bool is_trigger);
	bool GetIsTrigger() { return is_trigger; }
	void SetBouncing(const float bouncing);
	float GetBouncing() { return bouncing; }

	void SetStaticFriction(const float v);
	float GetStaticFriction() { return static_friction; }
	void SetDynamicFriction(const float v);
	float GetDynamicFriction() { return dynamic_friction; }
	void SetFrictionCombineMode(CombineMode mode);
	CombineMode GetFrictionCombineMode() { return friction_combine; }
	void SetBouncingCombineMode(CombineMode mode);
	CombineMode GetBouncingCombineMode() { return friction_combine; }

	virtual void SetCollisionLayer(std::string layer);
	std::string GetCollisionLayer();

protected:

	void InitCollider();
	void InitMaterial();
	inline bool IsController() { return (type == ComponentType::CHARACTER_CONTROLLER); }
	inline void BeginUpdateShape(bool force_update = false);
	inline void EndUpdateShape(bool force_update = false);

	const float3 GetLocalMeshAabbSize() const;
	const AABB GetLocalMeshAabb() const;
	const ComponentMesh* GetMesh() const;

	void OnEnable();
	void OnDisable();
	virtual void Update();

	bool DrawInspector();
	void DrawLayersCombo();
	void DrawCombineModeCombo(CombineMode& current_mode, int mode); // 0- friction 1- bouncing
	void HandleAlienEvent(const AlienEvent& e);

	virtual void DrawSpecificInspector() {}
	virtual void DrawScene(ComponentCamera* camera) override;
	virtual void Reset();
	virtual void Clone(Component* clone) {}
	virtual void SetComponent(Component* component) {}
	virtual void SaveComponent(JSONArraypack* to_save);
	virtual void LoadComponent(JSONArraypack* to_load);
	virtual void ScaleChanged() {};

protected:

	bool force_update = false;
	std::string layer_name = "Default";
	int layer_num = 0;

	PxShape*    shape = nullptr;
	float3		center = float3::zero();
	float3	    rotation = float3::zero();
	bool		is_trigger = false;

	CombineMode friction_combine = CombineMode::Average;
	CombineMode bouncing_combine = CombineMode::Average;
	PxMaterial* material = nullptr;
	float		bouncing = 0.f;
	float		static_friction = 0.f;
	float		dynamic_friction = 0.f;

	const char* mode_names[4] = { "Average","Minimum","Multiply","Maximum" };
};

class ContactPoint
{
public:

	ContactPoint(const float3& normal, const float3& point, float separation, ComponentCollider* this_collider, ComponentCollider* other_collider);

	float3				normal = float3(0.f, 0.f, 0.f);
	float3				point = float3(0.f, 0.f, 0.f);
	float				separation = 0.f;
	ComponentCollider* this_collider = nullptr;
	ComponentCollider* other_collider = nullptr;
};

class Collision
{
public:

	Collision(ComponentCollider* collider, ComponentRigidBody* rigid_body, ComponentTransform* transform, const std::vector<ContactPoint>& contancts, uint num_contact, GameObject* game_object, const float3& impulse, const float3& relative_velocity);

	ComponentCollider*	collider = nullptr;
	ComponentRigidBody* rigid_body = nullptr;
	ComponentTransform* transform = nullptr;
	std::vector<ContactPoint>	contancts;
	uint						num_contact = 0u;
	GameObject* game_object = nullptr;
	float3                      impulse = float3(0.f, 0.f, 0.f);
	float3                      relative_velocity = float3(0.f, 0.f, 0.f);

};