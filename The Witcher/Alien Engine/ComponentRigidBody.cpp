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
	physics->AddRigidBody(this);
	Reset();
	App->SendAlienEvent(this, AlienEventType::RIGIDBODY_ADDED);
}

ComponentRigidBody::~ComponentRigidBody()
{
	App->SendAlienEvent(this, AlienEventType::RIGIDBODY_DELETED);
}

//void ComponentRigidBody::OnEnable()
//{
//	App->SendAlienEvent(this, AlienEventType::RIGIDBODY_ENABLED);
//}
//
//void ComponentRigidBody::OnDisable()
//{
//	App->SendAlienEvent(this, AlienEventType::RIGIDBODY_DISABLED);
//}

void ComponentRigidBody::Update()
{

}

bool ComponentRigidBody::DrawInspector()
{

	static bool check;
	check = enabled;

	ImGui::PushID(this);
	//if (ImGui::Checkbox("##CmpActive", &check)) {
	//	SetEnable(check);
	//}
	//ImGui::SameLine();

	bool current_is_kinematic = is_kinematic;
	float current_mass = mass;
	float current_drag = drag;
	float current_angular_drag = angular_drag;


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

void ComponentRigidBody::Reset()
{
	bool values[3] = { false, false, false };

	SetIsKinematic(false);
	SetMass(1.f);
	SetDrag(0.f);
	SetAngularDrag(0.1f);
	SetFreezePosition(values);
	SetFreezeRotation(values);
}

void ComponentRigidBody::Clone(Component* clone)
{
	ComponentRigidBody* rb = (ComponentRigidBody*)clone;

	rb->SetIsKinematic(is_kinematic);
	rb->SetMass(mass);
	rb->SetDrag(drag);
	rb->SetAngularDrag(angular_drag);
	rb->SetFreezePosition(freeze_position);
	rb->SetFreezeRotation(freeze_rotation);
}

void ComponentRigidBody::SaveComponent(JSONArraypack* to_save)
{
	//to_save->SetBoolean("Enabled", enabled);
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
	//enabled = to_load->GetBoolean("Enabled");
	//if (enabled == false)
	//{
	//	OnDisable();
	//}

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

void ComponentRigidBody::AddTorque(const float3 force, ForceMode mode, Space space)
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

// Rigid Body Values ----------------------------

float3 ComponentRigidBody::GetPosition()
{
	return PXVEC3_TO_F3(body->getGlobalPose().p);
}

Quat ComponentRigidBody::GetRotation()
{
	return PXQUAT_TO_QUAT(body->getGlobalPose().q);
}

void ComponentRigidBody::SetPosition(const float3 new_position)
{
	PxTransform trans = body->getGlobalPose();
	trans.p = F3_TO_PXVEC3(new_position);
	body->setGlobalPose(trans);
	transform->SetGlobalPosition(PXVEC3_TO_F3(trans.p));
	transform->SetGlobalRotation(PXQUAT_TO_QUAT(trans.q));
}

void ComponentRigidBody::SetRotation(const Quat new_rotation)
{
	PxTransform trans = body->getGlobalPose();
	trans.q = QUAT_TO_PXQUAT(new_rotation);
	body->setGlobalPose(trans);
	transform->SetGlobalPosition(PXVEC3_TO_F3(trans.p));
	transform->SetGlobalRotation(PXQUAT_TO_QUAT(trans.q));

}

void ComponentRigidBody::SetTransform(const float3 new_position, const Quat new_rotation)
{
	PxTransform trans(F3_TO_PXVEC3(new_position), QUAT_TO_PXQUAT(new_rotation));
	body->setGlobalPose(trans);
	transform->SetGlobalPosition(PXVEC3_TO_F3(trans.p));
	transform->SetGlobalRotation(PXQUAT_TO_QUAT(trans.q));
}

void ComponentRigidBody::SetIsKinematic(const bool value)
{
	is_kinematic = value;

	(is_kinematic)
		? body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true)
		: body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
}

void ComponentRigidBody::SetMass(const float value)
{
	mass = value;
	PxRigidBodyExt::setMassAndUpdateInertia(*body, mass);
}

void ComponentRigidBody::SetDrag(const float value)
{
	drag = value;
	body->setLinearDamping(value);
}

void ComponentRigidBody::SetAngularDrag(const float value)
{
	angular_drag = value;
	body->setAngularDamping(value);
}

void ComponentRigidBody::SetFreezePosition(bool values[3])
{
	freeze_position[0] = values[0], freeze_position[1] = values[1], freeze_position[2] == values[2];

	body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, freeze_position[0]);
	body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, freeze_position[1]);
	body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, freeze_position[2]);
}

void ComponentRigidBody::GetFreezePosition(bool values[3])
{
	values = freeze_position;
}

void ComponentRigidBody::SetFreezeRotation(bool values[3])
{
	freeze_rotation[0] = values[0], freeze_rotation[1] = values[1], freeze_rotation[2] = values[2];

	body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, freeze_rotation[0]);
	body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, freeze_rotation[1]);
	body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, freeze_rotation[2]);
}

void ComponentRigidBody::GetFreezeRotation(bool values[3])
{
	values = freeze_rotation;
}

void ComponentRigidBody::SetVelocity(float3 velocity)
{
	body->setLinearVelocity(F3_TO_PXVEC3(velocity));
}

float3 ComponentRigidBody::GetVelocity()
{
	return  PXVEC3_TO_F3(body->getLinearVelocity());
}

void ComponentRigidBody::SetAngularVelocity(float3 velocity)
{
	body->setAngularVelocity(F3_TO_PXVEC3(velocity));
}

float3 ComponentRigidBody::GetAngularVelocity()
{
	return  PXVEC3_TO_F3(body->getAngularVelocity());
}

void ComponentRigidBody::SetBodyProperties()
{
	body = physics->actor->is<PxRigidDynamic>();
	SetIsKinematic(is_kinematic);
	SetMass(mass);
	SetDrag(drag);
	SetAngularDrag(angular_drag);
	SetFreezePosition(freeze_position);
	SetFreezeRotation(freeze_rotation);
}
