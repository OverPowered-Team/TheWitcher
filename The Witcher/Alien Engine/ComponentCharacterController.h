#pragma once

#include "ComponentCollider.h"
#include "MathGeoLib/include/Math/MathAll.h"
#include "PhysX/include/characterkinematic/PxController.h"
#include "PhysX/include/characterkinematic/PxCapsuleController.h"

class GameObject;
class ComponentMesh;
class ComponentTransform;

struct ControllerColliderHit
{
	ComponentCollider* collider = nullptr;
	ComponentCharacterController* controller = nullptr;
	GameObject* gameObject = nullptr;
	float3 moveDirection = float3::zero();
	float moveLength = 0.0f;
	float3 normal = float3::zero();
	float3 point = float3::zero();
	ComponentRigidBody* rigidbody = nullptr;
	ComponentTransform* transform = nullptr;
};

class UserControllerHitReport : public PxUserControllerHitReport
{
	friend class ComponentCharacterController;
private:

	virtual void onShapeHit(const PxControllerShapeHit& hit);

	virtual void onControllerHit(const PxControllersHit& hit);

	virtual void onObstacleHit(const PxControllerObstacleHit& hit);

private:
	ComponentCharacterController* controller = nullptr;
};

class __declspec(dllexport) ComponentCharacterController : public ComponentCollider
{
	friend class GameObject;
	friend class ModuleObjects;
	friend class ModulePhysX;
	friend class ReturnZ;
	friend class CompZ;
	friend class ComponentRigidBody;
	friend class UserControllerHitReport;
	friend class ComponentPhysics;

public:

	ComponentCharacterController(GameObject* go);
	virtual ~ComponentCharacterController();


	void SetCharacterOffset(float3 offset);
	float GetCharacterHeight() { return desc.height; }
	void SetCharacterHeight(const float height);
	float GetCharacterRadius() { return desc.radius; }
	void SetCharacterRadius(const float radius);
	void SetSlopeLimit(const float slopeLimit);
	float GetSlopeLimit() const;
	void SetStepOffset(const float stepOffset);
	void SetContactOffset(const float contactOffset);

	// translates without overlap check
	bool SetPosition(float3 position) const;
	bool SetFootPosition(float3 position) const;
	float3 GetPosition() const;
	float3 GetFootPosition() const;
	// TODO: make own copy of collision flag to not work with physx data outside.

	// Move by motion, this function doesn't apply any gravity,
	// (user implementation, or active force gravity in character controller component)
	PxControllerCollisionFlags Move(float3 motion);
	void SetCollisionLayer(std::string layer);

private:

	bool CharacterEnabled();

	void CreateController();
	void DeleteController();

	void SetDefaultConf();
	void OnControllerColliderHit(ControllerColliderHit hit);
	void LinkShapesToComponent();
	void UpdateParameters(); // called when object is enabled, just in case to refresh some changed data

	void OnEnable();
	void OnDisable();
	void Clone(Component* clone) {}
	void Update();
	void DrawScene() override;
	bool DrawInspector();

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

public:

	PxControllerCollisionFlags collisionFlags;
	// The velocity returned is simply the difference in distance 
	// for the current timestep before and after a call to CharacterController.Move
	PxExtendedVec3 velocity;
	bool isGrounded = false;

private:

	PxController* controller = nullptr;
	UserControllerHitReport* report = nullptr;
	PxCapsuleControllerDesc desc;

	float min_distance = 0.0f;
	float dynamic_friction = 0.5f;
	float static_friction = 0.5f;
	float restitution = 0.6f;

	float gravity = 20.0f;
	float3 move_direction = float3::zero();
	float3 controller_offset = float3::zero();

	// Advanced options -------------

	// Forces move with zero vector when controller is idle, 
	// this forces messages from OnControllerColliderHit in any situation,
	// if off, messages from this callback only happen when a move is performed.
	bool force_move = false;
	// if off, any gravity needs to be implemented by the user
	// if on, forces gravity defined on gravity field always
	// that isGrounded is false
	bool force_gravity = false;
};


//void Jump(float3 direction= float3::zero());
//bool CanJump();
//bool OnGround();

//float GetJumpSpeed() { return jump_speed; }
//void SetJumpSpeed(const float jump_speed);
//float GetGravity() { return gravity; }
//void SetGravity(const float fall_speed);

//void ApplyImpulse(float3 direction = float3::zero());
//void SetWalkDirection(float3 direction);

//void SetRotation(const Quat rotation);
//Quat GetRotation() const;

//void SetPosition(const float3 pos);
//float3 GetPosition() const;