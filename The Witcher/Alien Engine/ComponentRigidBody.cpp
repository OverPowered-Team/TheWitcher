#include "Application.h"
#include "ComponentPhysics.h"
#include "ComponentRigidBody.h"
#include "ComponentTransform.h"
#include "ComponentCollider.h"
#include "GameObject.h"
#include "Time.h"
#include "ModuleInput.h"
#include "Event.h"
#include "mmgr/mmgr.h"


ComponentRigidBody::ComponentRigidBody(GameObject* go) : ComponentBasePhysic(go)
{
	type = ComponentType::RIGID_BODY;

	SetMass(1.0f);
	SetDrag(0.f);
	SetAngularDrag(0.f);

	App->SendAlienEvent(this, AlienEventType::RIGIDBODY_ADDED);
}

ComponentRigidBody::~ComponentRigidBody()
{
	App->SendAlienEvent(this, AlienEventType::RIGIDBODY_DELETED);
}

void ComponentRigidBody::OnEnable()
{
	App->SendAlienEvent(this, AlienEventType::RIGIDBODY_ENABLED);
}

void ComponentRigidBody::OnDisable()
{
	App->SendAlienEvent(this, AlienEventType::RIGIDBODY_DISABLED);
}

void ComponentRigidBody::Update()
{

}

bool ComponentRigidBody::DrawInspector()
{

	static bool check;
	check = enabled;

	ImGui::PushID(this);

	if (ImGui::Checkbox("##CmpActive", &check)) {
		SetEnable(check);
	}

	bool current_is_kinematic = is_kinematic;
	float current_mass = mass;
	float current_drag = drag;
	float current_angular_drag = angular_drag;

	ImGui::SameLine();

	// RigidBody Config --------------------------------------

	if (ImGui::CollapsingHeader("Rigid Body", &not_destroy))
	{
		ImGui::Spacing();
		ImGui::Title("Is Kinematic", 1);	if (ImGui::Checkbox("##is_kinematic", &current_is_kinematic)) { SetIsKinematic(current_is_kinematic); }
		ImGui::Title("Mass", 1);			if (ImGui::DragFloat("##mass", &current_mass, 0.01f, 0.00f, FLT_MAX)) { SetMass(current_mass); }
		ImGui::Title("Drag", 1);			if (ImGui::DragFloat("##drag", &current_drag, 0.01f, 0.00f, FLT_MAX)) { SetDrag(current_drag); }
		ImGui::Title("Angular Drag", 1);	if (ImGui::DragFloat("##angular_drag", &current_angular_drag, 0.01f, 0.00f, FLT_MAX)) { SetAngularDrag(current_angular_drag); }

		ImGui::Title("Freeze", 1);	ImGui::Text("");
		ImGui::Spacing();
		ImGui::PushID(freeze_position);
		ImGui::Title("Position", 2);
		ImGui::Checkbox("X", &freeze_position[0]); ImGui::SameLine();
		ImGui::Checkbox("Y", &freeze_position[1]); ImGui::SameLine();
		ImGui::Checkbox("Z", &freeze_position[2]);
		ImGui::PopID();
		ImGui::PushID(freeze_rotation);
		ImGui::Title("Rotation", 2);
		ImGui::Checkbox("X", &freeze_rotation[0]); ImGui::SameLine();
		ImGui::Checkbox("Y", &freeze_rotation[1]); ImGui::SameLine();
		ImGui::Checkbox("Z", &freeze_rotation[2]);
		ImGui::PopID();
		ImGui::Spacing();
	}

	ImGui::PopID();

	return true;
}

void ComponentRigidBody::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetBoolean("Enabled", enabled);
	to_save->SetNumber("Type", (int)type);

	to_save->SetNumber("IsKinematic", is_kinematic);
	to_save->SetNumber("Mass", mass);
	to_save->SetNumber("Drag", drag);
	to_save->SetNumber("AngularDrag", angular_drag);

	to_save->SetBoolean("FreezePosX", freeze_position[0]);
	to_save->SetBoolean("FreezePosY", freeze_position[1]);
	to_save->SetBoolean("FreezePosZ", freeze_position[2]);

	to_save->SetBoolean("FreezeRotX", freeze_rotation[0]);
	to_save->SetBoolean("FreezeRotY", freeze_rotation[1]);
	to_save->SetBoolean("FreezeRotZ", freeze_rotation[2]);
}

void ComponentRigidBody::LoadComponent(JSONArraypack* to_load)
{
	enabled = to_load->GetBoolean("Enabled");
	if (enabled == false)
	{
		OnDisable();
	}

	SetIsKinematic(to_load->GetNumber("IsKinematic"));
	SetMass(to_load->GetNumber("Mass"));
	SetDrag(to_load->GetNumber("Drag"));
	SetAngularDrag(to_load->GetNumber("AngularDrag"));

	freeze_position[0] = to_load->GetBoolean("FreezePosX");
	freeze_position[1] = to_load->GetBoolean("FreezePosY");
	freeze_position[2] = to_load->GetBoolean("FreezePosZ");

	freeze_rotation[0] = to_load->GetBoolean("FreezeRotX");
	freeze_rotation[1] = to_load->GetBoolean("FreezeRotY");
	freeze_rotation[2] = to_load->GetBoolean("FreezeRotZ");
}

// Forces Functions ------------------------------

void ComponentRigidBody::AddForce(const float3 force, ForceMode mode, Space space)
{
	if (CanUseRigidBody())
	{
		PxVec3 force_vec = F3_TO_PXVEC3(force);

		if (space == Space::Local)
			force_vec = body->getGlobalPose().rotate(force_vec);

		switch (mode)
		{
		case ForceMode::FORCE:
			body->addForce(force_vec, PxForceMode::eFORCE);
			break;
		case ForceMode::IMPULSE:
			body->addForce(force_vec, PxForceMode::eIMPULSE);
			break;
		case ForceMode::ACCELERATION:
			body->addForce(force_vec, PxForceMode::eACCELERATION);
			break;
		case ForceMode::VELOCITY_CHANGE:
			body->addForce(force_vec, PxForceMode::eVELOCITY_CHANGE);
			break;
		}
	}
}

void ComponentRigidBody::AddTorque(const float3 force, ForceMode mode, Space space)
{
	if (CanUseRigidBody())
	{
		PxVec3 force_vec = F3_TO_PXVEC3(force);

		if (space == Space::Local)
			force_vec = body->getGlobalPose().rotate(force_vec);

		switch (mode)
		{
		case ForceMode::FORCE:
			body->addTorque(force_vec, PxForceMode::eFORCE);
			break;
		case ForceMode::IMPULSE:
			body->addTorque(force_vec, PxForceMode::eIMPULSE);
			break;
		case ForceMode::ACCELERATION:
			body->addTorque(force_vec, PxForceMode::eACCELERATION);
			break;
		case ForceMode::VELOCITY_CHANGE:
			body->addTorque(force_vec, PxForceMode::eVELOCITY_CHANGE);
			break;
		}
	}
}

// Rigid Body Values ----------------------------

float3 ComponentRigidBody::GetPosition()
{
	return (CanUseRigidBody())
		? PXVEC3_TO_F3(body->getGlobalPose().p)
		: transform->GetGlobalPosition();
}

Quat ComponentRigidBody::GetRotation()
{
	return (CanUseRigidBody())
		? PXQUAT_TO_QUAT(body->getGlobalPose().q)
		: transform->GetGlobalRotation();
}

void ComponentRigidBody::SetPosition(const float3 new_position)
{
	if (CanUseRigidBody())
	{
		PxTransform trans = body->getGlobalPose();
		trans.p = F3_TO_PXVEC3(new_position);
		body->setGlobalPose(trans);
		transform->SetGlobalPosition(PXVEC3_TO_F3(trans.p));
		transform->SetGlobalRotation(PXQUAT_TO_QUAT(trans.q));
	}
}

void ComponentRigidBody::SetRotation(const Quat new_rotation)
{
	if (CanUseRigidBody())
	{
		PxTransform trans = body->getGlobalPose();
		trans.q = QUAT_TO_PXQUAT(new_rotation);
		body->setGlobalPose(trans);
		transform->SetGlobalPosition(PXVEC3_TO_F3(trans.p));
		transform->SetGlobalRotation(PXQUAT_TO_QUAT(trans.q));
	}
}

void ComponentRigidBody::SetTransform(const float3 new_position, const Quat new_rotation)
{
	if (CanUseRigidBody())
	{
		PxTransform trans(F3_TO_PXVEC3(new_position), QUAT_TO_PXQUAT(new_rotation));
		body->setGlobalPose(trans);
		transform->SetGlobalPosition(PXVEC3_TO_F3(trans.p));
		transform->SetGlobalRotation(PXQUAT_TO_QUAT(trans.q));
	}
}

void ComponentRigidBody::SetIsKinematic(const bool value)
{
	is_kinematic = value;

	if (CanUseRigidBody())
		(is_kinematic)
		? body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true)
		: body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
}

void ComponentRigidBody::SetMass(const float value)
{
	mass = value;
	if (CanUseRigidBody())
		PxRigidBodyExt::setMassAndUpdateInertia(*body, mass);
}

void ComponentRigidBody::SetDrag(const float value)
{
	drag = value;

	if (CanUseRigidBody())
		body->setLinearDamping(value);
}

void ComponentRigidBody::SetAngularDrag(const float value)
{
	angular_drag = value;

	if (CanUseRigidBody())
		body->setAngularDamping(value);
}

void ComponentRigidBody::SetFreezePosition(bool values[3])
{
	freeze_position[0] = values[0], freeze_position[1] = values[1], freeze_position[2] == values[2];

	if (CanUseRigidBody())
	{
		body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, freeze_position[0]);
		body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, freeze_position[1]);
		body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, freeze_position[2]);
	}

}

void ComponentRigidBody::GetFreezePosition(bool values[3])
{
	values = freeze_position;
}

void ComponentRigidBody::SetFreezeRotation(bool values[3])
{
	freeze_rotation[0] = values[0], freeze_rotation[1] = values[1], freeze_rotation[2] = values[2];

	if (CanUseRigidBody())
	{
		body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, freeze_rotation[0]);
		body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, freeze_rotation[1]);
		body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, freeze_rotation[2]);
	}
}

void ComponentRigidBody::GetFreezeRotation(bool values[3])
{
	values = freeze_rotation;
}

void ComponentRigidBody::SetVelocity(float3 velocity)
{
	if (CanUseRigidBody())
		body->setLinearVelocity(F3_TO_PXVEC3(velocity));
}

float3 ComponentRigidBody::GetVelocity()
{
	return (CanUseRigidBody())
		? PXVEC3_TO_F3(body->getLinearVelocity())
		: float3::zero();
}

void ComponentRigidBody::SetAngularVelocity(float3 velocity)
{
	if (CanUseRigidBody())
		body->setAngularVelocity(F3_TO_PXVEC3(velocity));
}

float3 ComponentRigidBody::GetAngularVelocity()
{
	return (CanUseRigidBody())
		? PXVEC3_TO_F3(body->getAngularVelocity())
		: float3::zero();
}

bool ComponentRigidBody::CanUseRigidBody()
{
	(physics->actor)
		? body = physics->actor->is<PxRigidDynamic>()
		: body = nullptr;

	return (body && enabled);
}

void ComponentRigidBody::SetBodyProperties()
{
	SetIsKinematic(is_kinematic);
	SetMass(mass);
	SetDrag(drag);
	SetAngularDrag(angular_drag);
	SetFreezePosition(freeze_position);
	SetFreezeRotation(freeze_rotation);
}
