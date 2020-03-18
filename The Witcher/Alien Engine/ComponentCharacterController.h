#pragma once

#include "Component.h"
#include "MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class GameObject;
class ModulePhysics;
class ComponentMesh;
class ComponentRigidBody;
class ComponentVehicle;
class ComponentTransform;
class btKinematicCharacterController;
class btPairCachingGhostObject;
class Alien;

class __declspec(dllexport) ComponentCharacterController : public Component
{
	friend class GameObject;
	friend class ModuleObjects;
	friend class ModulePhysics;
	friend class ReturnZ;
	friend class CompZ;
	friend class ComponentRigidBody;

public:

	ComponentCharacterController(GameObject* go);
	virtual ~ComponentCharacterController();

	void Jump(float3 direction= float3::zero());
	bool CanJump();

	float GetJumpSpeed() { return jump_speed; }
	void SetJumpSpeed(const float jump_speed);
	float GetGravity() { return gravity; }
	void SetGravity(const float fall_speed);

	void ApplyImpulse(float3 direction = float3::zero());
	void SetWalkDirection(float3 direction);
	void SetRotation(const Quat rotation);

	void SetCharacterOffset(const float3 offset);
	float GetCharacterHeight() { return character_height; }
	void SetCharacterHeight(const float height);
	float GetCharacterRadius() { return character_radius; }
	void SetCharacterRadius(const float radius);

protected:

	void RecreateCapusle();

	void Update();
	void DrawScene();
	bool DrawInspector();

	void Reset();
	void Clone(Component* clone) {}
	void SetComponent(Component* component) {}
	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

protected:
	ComponentTransform* transform = nullptr;
	btKinematicCharacterController* controller = nullptr;
	btPairCachingGhostObject* body = nullptr;
	btCapsuleShape* shape = nullptr;

	float3 character_offset = float3::zero();
	float character_height = 1.f;
	float character_radius = 0.5f;

	float jump_speed = 0.f;
	float gravity = 80.f;


	bool test = false;
};