#pragma once
#include "Component.h"
#include "PxShape.h"
#include <list>

using namespace physx;

class CollisionLayers;

class __declspec(dllexport) ComponentPhysics : public Component
{
	friend class GameObject;
	friend class SimulationEventCallback;

	friend class ModuleObjects;
	friend class ModulePhysX;
	friend class ComponentTransform;
	friend class ComponentCollider;
	friend class ComponentBoxCollider;
	friend class ComponentSphereCollider;
	friend class ComponentCapsuleCollider;
	friend class ComponentMeshCollider;
	friend class ComponentConvexHullCollider;
	friend class ComponentCharacterController;
	friend class ComponentRigidBody;
	friend class ComponentJoint;
	friend class ComponentConfigurableJoint;
	friend class UserControllerHitReport;
	friend class RaycastHit;
public:

	ComponentPhysics(GameObject* go);
	virtual ~ComponentPhysics();
	ComponentRigidBody* GetRigidBody();

private:

	void OnEnable();
	void OnDisable();

	void Update();
	void PostUpdate();
	void HandleAlienEvent(const AlienEvent& e);

	void AttachCollider(ComponentCollider* collider, bool to_update = false);    // To Update if is only a removement 
	void DettachCollider(ComponentCollider* collider, bool to_update = false);	 // to update the shape
	bool AddCollider(ComponentCollider* collider);     
	bool RemoveCollider(ComponentCollider* collider);	
 	bool FindCollider( ComponentCollider* collider);
	bool CheckCollider(ComponentCollider* collider);
	
	bool AddRigidBody(ComponentRigidBody* rb);
	bool RemoveRigidBody(ComponentRigidBody* rb);
	void SwitchedRigidBody(ComponentRigidBody* rb);
	bool CheckRigidBody(ComponentRigidBody* rb);

	bool AddController(ComponentCharacterController* ctrl);
	bool RemoveController(ComponentCharacterController* ctrl);
	void SwitchedController(ComponentCharacterController* ctrl);
	bool CheckController(ComponentCharacterController* ctrl);

	void GizmoManipulation();
	void UpdatePositioning();
	void WakeUp();
	void PutToSleep();
	void ChangedFilters();

	bool CheckChangeState();
	void UpdateBody();
	float3 GetValidPhysicScale();

	bool HasEnabledColliders();
	bool ShapeAttached(PxShape* shape);
	bool IsDynamic();
	bool IsKinematic();
	bool IsDisabled();


protected:

	enum class PhysicState { STATIC = 0, DYNAMIC, CTRL_CHARACTER, CTRL_VEHICLE, INVALID_TRANS, DISABLED}
	state = PhysicState::DISABLED;

	bool gizmo_selected = false;

	PxU32							physic_ID = 0;
	GameObject*						go = nullptr;
	ComponentTransform*				transform = nullptr;
	std::list<ComponentCollider*>	colliders;
	std::list<ComponentScript*>		scripts;
	ComponentCharacterController*	controller = nullptr;
	ComponentRigidBody*				rigid_body = nullptr;
	PxRigidActor*					actor = nullptr;
	CollisionLayers*				layers = nullptr;
	float3							scale = float3::one();
};

