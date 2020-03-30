#include "Application.h"
#include "ComponentRigidBody.h"
#include "ComponentTransform.h"
#include "ComponentCollider.h"
#include "GameObject.h"
#include "Time.h"
#include "ModuleInput.h"
#include "Event.h"
#include "mmgr/mmgr.h"

ComponentRigidBody::ComponentRigidBody(GameObject* go) : Component(go)
{
	App->SendAlienEvent(this, AlienEventType::RIGIDBODY_ADDED);

	// GameObject Components 
	type = ComponentType::RIGID_BODY;
	transform = (transform == nullptr) ? game_object_attached->GetComponent<ComponentTransform>() : transform;

	// Create aux shape 

	aux_shape = new btBoxShape(btVector3(1.f, 1.f, 1.f));

	// Create body 

	btRigidBody::btRigidBodyConstructionInfo rbInfo(1.f, nullptr, aux_shape);
	body = new btRigidBody(rbInfo);
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	body->setUserPointer(this);

	// Default values 

	for (uint i = 0; i < (uint)ForceMode::MAX; ++i)
	{
		force_to_apply[i] = float3::zero();
		torque_to_apply[i] = float3::zero();
	}

	SetMass(1.0f);
	SetDrag(0.f);
	SetAngularDrag(0.f);

	// Search Collider 

	ComponentCollider* new_coll = game_object_attached->GetComponent<ComponentCollider>();

	if (new_coll != nullptr)
	{
		AddCollider(new_coll);
	}

	SetBodyTranform((collider)
		? transform->GetGlobalPosition() + collider->GetWorldCenter()
		: transform->GetGlobalPosition(),
		transform->GetGlobalRotation());

	App->physics->AddBody(body);
}

ComponentRigidBody::~ComponentRigidBody()
{
	App->SendAlienEvent(this, AlienEventType::RIGIDBODY_DELETED);

	if (collider)
	{
		RemoveCollider();
	}

	for (int i = 0; i < body->getNumConstraintRefs(); ++i)
	{
		//C_JointP2P * joint =(C_JointP2P *) body->getConstraintRef(i)->getUserConstraintPtr();
		//joint->BodyDeleted(linked_go);
		body->removeConstraintRef(body->getConstraintRef(i));
	}

	App->physics->RemoveBody(body);

	delete body;
	delete aux_shape;
}

void ComponentRigidBody::Update()
{
	// Set Go Transform ---------------------------

	if (Time::GetGameState() != Time::GameState::NONE)
	{
		btTransform bt_transform = body->getCenterOfMassTransform();
		btQuaternion rotation = bt_transform.getRotation();
		btVector3 position = (collider) ? bt_transform.getOrigin() - ToBtVector3(collider->GetWorldCenter()) : bt_transform.getOrigin();

		body->activate(true);
		transform->SetGlobalPosition(float3(position));
		transform->SetGlobalRotation(math::Quat(rotation));
	}
	else
	{
		SetBodyTranform((collider)
			? transform->GetGlobalPosition() + collider->GetWorldCenter()
			: transform->GetGlobalPosition(),
			transform->GetGlobalRotation());
	}

	btVector3 freeze_p((float)!freeze_position[0], (float)!freeze_position[1], (float)!freeze_position[2]);
	btVector3 freeze_r((float)!freeze_rotation[0], (float)!freeze_rotation[1], (float)!freeze_rotation[2]);

	if (body->getLinearFactor() != freeze_p)
	{
		body->activate(true);
		body->setLinearFactor(freeze_p);
	}
	if (body->getAngularFactor() != freeze_r)
	{
		body->activate(true);
		body->setAngularFactor(freeze_r);
	}

	//	// Apply Forces ----------------------

	for (uint i = 0; i < (uint)ForceMode::MAX; ++i)
	{
		if (!force_to_apply[i].Equals(float3::zero()))
		{
			switch ((ForceMode)i)
			{
			case ForceMode::FORCE:
				body->applyCentralForce(ToBtVector3(force_to_apply[i]));
				break;
			case ForceMode::IMPULSE:
				body->applyCentralImpulse(ToBtVector3(force_to_apply[i]));
				break;
			}
			force_to_apply[i] = float3::zero();
		}
	}

	for (uint i = 0; i < (uint)ForceMode::MAX; ++i)
	{
		if (!torque_to_apply[i].Equals(float3::zero()))
		{
			switch ((ForceMode)i)
			{
			case ForceMode::FORCE:
				body->applyTorque(ToBtVector3(torque_to_apply[i]));
				break;
			case ForceMode::IMPULSE:
				body->applyTorqueImpulse(ToBtVector3(torque_to_apply[i]));
				break;
			}

			torque_to_apply[i] = float3::zero();
		}
	}
}

bool ComponentRigidBody::DrawInspector()
{
	bool current_is_kinematic = is_kinematic;
	float current_mass = mass;
	float current_drag = drag;
	float current_angular_drag = angular_drag;


	// RigidBody Config --------------------------------------

	if (ImGui::CollapsingHeader("Rigid Body", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
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

	return true;
}


void ComponentRigidBody::Reset()
{
	for (uint i = 0; i < (uint)ForceMode::MAX; ++i)
	{
		force_to_apply[i].zero();
		torque_to_apply[i].zero();
	}

	SetMass(1.0f);
	SetDrag(0.f);
	SetAngularDrag(0.f);
}

void ComponentRigidBody::Clone(Component* clone)
{

}

void ComponentRigidBody::SaveComponent(JSONArraypack* to_save)
{
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
	float3 final_force = force;

	if (space == Space::Local)
	{
		final_force = transform->global_transformation.RotatePart().Mul(final_force);
	}

	force_to_apply[(uint)mode] += final_force;
}

void ComponentRigidBody::AddTorque(const float3 force, ForceMode mode, Space space)
{
	float3 final_force = force;

	if (space == Space::Local)
	{
		final_force = transform->global_transformation.RotatePart().Mul(final_force);
	}

	torque_to_apply[(uint)mode] += final_force;
}

// Rigid Body Values ----------------------------

void ComponentRigidBody::SetIsKinematic(const bool value)
{
	is_kinematic = value;
	(is_kinematic)
		? body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT)
		: body->setCollisionFlags(body->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
}

void ComponentRigidBody::SetMass(const float value)
{
	mass = value;
	UpdateBodyInertia();
}

void ComponentRigidBody::SetDrag(const float value)
{
	drag = value;
	body->setDamping(drag, angular_drag);
}

void ComponentRigidBody::SetAngularDrag(const float value)
{
	angular_drag = value;
	body->setDamping(drag, angular_drag);
}

void ComponentRigidBody::SetPosition(float3 pos)
{
	btTransform trans = body->getCenterOfMassTransform();
	trans.setOrigin(btVector3(pos.x, pos.y, pos.z));
	body->setCenterOfMassTransform(trans);
}

float3 ComponentRigidBody::GetPosition() const
{
	btTransform trans = body->getCenterOfMassTransform();
	btVector3 pos = trans.getOrigin();
	return float3(pos.x(), pos.y(), pos.z());
}

void ComponentRigidBody::SetRotation(const Quat rot)
{
	btTransform trans = body->getCenterOfMassTransform();
	trans.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
	body->setCenterOfMassTransform(trans);
}

Quat ComponentRigidBody::GetRotation() const
{
	btTransform trans = body->getCenterOfMassTransform();
	btQuaternion rot = trans.getRotation();
	return Quat(rot.x(), rot.y(), rot.z(), rot.w());
}

void ComponentRigidBody::SetVelocity(float3 velocity)
{
	body->setLinearVelocity(ToBtVector3(velocity));
}

float3 ComponentRigidBody::GetVelocity()
{
	return float3(&body->getLinearVelocity()[0]);
}

void ComponentRigidBody::SetAngularVelocity(float3 velocity)
{
	body->setAngularVelocity(ToBtVector3(velocity));
}
float3 ComponentRigidBody::GetAngularVelocity()
{
	return float3(&body->getAngularVelocity()[0]);
}

void ComponentRigidBody::SetBodyTranform(const float3& pos, const Quat& rot)
{
	body->setWorldTransform(ToBtTransform(pos, rot));
}

void ComponentRigidBody::AddCollider(ComponentCollider* new_coll)
{
	if (collider == nullptr)
	{
		collider = new_coll;
		collider->rigid_body = this;

		App->physics->RemoveBody(collider->aux_body);
		body->setCollisionFlags(body->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
		body->setCollisionShape(collider->shape);
		UpdateBodyInertia();

		// Set this body properties 

		collider->SetIsTrigger(collider->is_trigger);
		collider->SetBouncing(collider->bouncing);
		collider->SetFriction(collider->friction);
		collider->SetAngularFriction(collider->angular_friction);
	}
}

void ComponentRigidBody::UpdateCollider()
{
	if (collider != nullptr)
	{
		body->setCollisionShape(collider->shape);
		UpdateBodyInertia();
	}
}

void ComponentRigidBody::RemoveCollider()
{
	if (collider != nullptr)
	{
		collider->SetIsTrigger(true);
		collider->SetBouncing(0.f);
		collider->SetFriction(0.f);
		collider->SetAngularFriction(0.f);
		body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

		App->physics->AddBody(collider->aux_body);
		body->setCollisionShape(aux_shape);

		collider->rigid_body = nullptr;
		collider = nullptr;

		UpdateBodyInertia();
	}
}

void ComponentRigidBody::UpdateBodyInertia()
{
	(collider)
		? collider->shape->calculateLocalInertia(mass, inertia)
		: aux_shape->calculateLocalInertia(mass, inertia);

	body->setMassProps(mass, inertia);
}
