#include "Application.h"
#include "ComponentCharacterController.h"
#include "ComponentTransform.h"
#include "ComponentPhysics.h"
#include "ComponentCollider.h"
#include "ComponentRigidBody.h"
#include "ComponentScript.h"
#include "ImGuizmos/ImGuizmo.h"
#include "CollisionLayers.h"
#include "GameObject.h"
#include "Alien.h"
#include "Event.h"
#include "Time.h"
#include "RandomHelper.h"
ComponentPhysics::ComponentPhysics(GameObject* go) : Component(go)
{
	this->go = go;
	serialize = false;  // Not save & load 
	ID = (PxU32)Random::GetRandom32ID();
	transform = go->GetComponent<ComponentTransform>();
	state = PhysicState::DISABLED;
	layers = &App->physx->layers;
	scale = GetValidPhysicScale();

	std::vector<ComponentScript*> found_script = go->GetComponents<ComponentScript>();
	for (ComponentScript* script : found_script)
		if (script->need_alien == true)
			scripts.push_back(script);
}

ComponentPhysics::~ComponentPhysics()
{
	if (state != PhysicState::DISABLED) {
		App->physx->RemoveBody(actor);
		actor = nullptr;
	}
	
	rigid_body = nullptr;
}

ComponentRigidBody* ComponentPhysics::GetRigidBody()
{
	return (rigid_body && IsDynamic()) ? rigid_body : nullptr;
}

void ComponentPhysics::Update()
{
	if (IsDisabled()) return;

	GizmoManipulation();  // Check if gizmo is selected
	UpdatePositioning();  // Move body or gameobject
}

void ComponentPhysics::PostUpdate()
{
	float3 current_scale = GetValidPhysicScale();

	if (!scale.Equals(current_scale)) {
		scale = current_scale;
		go->SendAlientEventThis(this, AlienEventType::PHYSICS_SCALE_CHANGED);
	}
}

void ComponentPhysics::HandleAlienEvent(const AlienEvent& e)
{
	switch (e.type) {
	case AlienEventType::SCRIPT_ADDED: {
		ComponentScript* script = (ComponentScript*)e.object;
		if (script->game_object_attached == game_object_attached && script->need_alien == true)
			scripts.push_back(script);
		break; }
	case AlienEventType::SCRIPT_DELETED: {
		ComponentScript* script = (ComponentScript*)e.object;
		if (script->game_object_attached == game_object_attached && script->need_alien == true)
			scripts.remove(script);
		break; }
	case AlienEventType::COLLIDER_ADDED: {
		ComponentCollider* object = (ComponentCollider*)e.object;
		AddCollider(object);
		break; }
	case AlienEventType::COLLIDER_DELETED: {
		ComponentCollider* object = (ComponentCollider*)e.object;
		RemoveCollider(object);
		break; }
	case AlienEventType::COLLIDER_ENABLED: {
		ComponentCollider* object = (ComponentCollider*)e.object;
		AttachCollider(object);
		break; }
	case AlienEventType::COLLIDER_DISABLED: {
		ComponentCollider* object = (ComponentCollider*)e.object;
		DettachCollider(object);
		break; }
	case AlienEventType::RIGIDBODY_ADDED: {
		ComponentRigidBody* object = (ComponentRigidBody*)e.object;
		AddRigidBody(object);
		break; }
	case AlienEventType::RIGIDBODY_DELETED: {
		ComponentRigidBody* object = (ComponentRigidBody*)e.object;
		RemoveRigidBody(object);
		break; }
	case AlienEventType::RIGIDBODY_ENABLED:
	case AlienEventType::RIGIDBODY_DISABLED: {
		ComponentRigidBody* object = (ComponentRigidBody*)e.object;
		SwitchedRigidBody(object);
		break; }
	case AlienEventType::CHARACTER_CTRL_ADDED: {
		ComponentCharacterController* object = (ComponentCharacterController*)e.object;
		AddController(object);
		break; }
	case AlienEventType::CHARACTER_CTRL_DELETED: {
		ComponentCharacterController* object = (ComponentCharacterController*)e.object;
		RemoveController(object);
		break; }
	case AlienEventType::CHARACTER_CTRL_ENABLED:
	case AlienEventType::CHARACTER_CTRL_DISABLED: {
		ComponentCharacterController* object = (ComponentCharacterController*)e.object;
		SwitchedController(object);
		break; }
	}
}

bool ComponentPhysics::AddRigidBody(ComponentRigidBody* rb)
{
	if (CheckRigidBody(rb))
	{
		rigid_body = rb;
		if (CheckChangeState()) UpdateBody();
		return true;
	}

	return true;
}

bool ComponentPhysics::RemoveRigidBody(ComponentRigidBody* rb)
{
	if (rb == rigid_body)
	{
		rigid_body = nullptr;
		if (CheckChangeState()) UpdateBody();
		return true;
	}

	return true;
}

void ComponentPhysics::SwitchedRigidBody(ComponentRigidBody* rb)
{
	if (rb == rigid_body)
		if (CheckChangeState()) UpdateBody();
}

void ComponentPhysics::AttachCollider(ComponentCollider* collider, bool only_update)
{
	bool do_attach = false;

	if (!only_update && CheckCollider(collider) && collider->IsEnabled())
	{
		if (CheckChangeState())
			UpdateBody();
		else
			do_attach = true;
	}

	if (actor && (do_attach || only_update))
	{
		if (!ShapeAttached(collider->shape))
			actor->attachShape(*collider->shape);
		if (IsDynamic())
			actor->is<PxRigidDynamic>()->wakeUp();
	}
}

void ComponentPhysics::DettachCollider(ComponentCollider* collider, bool only_update)
{
	bool do_dettach = false;

	if (!only_update && CheckCollider(collider) && !collider->IsEnabled())
	{
		if (CheckChangeState())
			UpdateBody();
		else
			do_dettach = true;
	}

	if (actor && (do_dettach || only_update))
	{
		if (ShapeAttached(collider->shape))
			actor->detachShape(*collider->shape);
		if (IsDynamic())
			actor->is<PxRigidDynamic>()->wakeUp();
	}
}

// Add Collider from Phisic System 
bool ComponentPhysics::AddCollider(ComponentCollider* collider)
{
	if (CheckCollider(collider))
	{
		if (!FindCollider(collider))			// Add Collider if is not found
			colliders.push_back(collider);
		if (CheckChangeState())					// Check if added collider change state
			UpdateBody();
		else									// Else only attach 
		{
			AttachCollider(collider, true);
		}
		return true;
	}
	return false;
}

// Remove Collider from Phisic System 
bool ComponentPhysics::RemoveCollider(ComponentCollider* collider)
{
	if (FindCollider(collider))
	{
		colliders.remove(collider);
		if (CheckChangeState()) UpdateBody();
		else
		{
			DettachCollider(collider, true);
		}
		return true;
	}
	return false;
}

bool ComponentPhysics::FindCollider(ComponentCollider* collider)
{
	return (std::find(colliders.begin(), colliders.end(), collider) != colliders.end());
}

bool ComponentPhysics::CheckCollider(ComponentCollider* collider)
{
	return (collider != nullptr && collider->game_object_attached == game_object_attached);
}

bool ComponentPhysics::CheckRigidBody(ComponentRigidBody* rb)
{
	return (rb != nullptr && rb->game_object_attached == game_object_attached);
}

// Controller Logic -------------------------------

bool ComponentPhysics::AddController(ComponentCharacterController* ctrl)
{
	if (CheckController(ctrl))
	{
		controller = ctrl;
		if (CheckChangeState()) UpdateBody();
		return true;
	}
	return false;
}

bool ComponentPhysics::RemoveController(ComponentCharacterController* ctrl)
{
	if (ctrl == controller)
	{
		controller = nullptr;
		if (CheckChangeState()) UpdateBody();
		return true;
	}
}

void ComponentPhysics::SwitchedController(ComponentCharacterController* ctrl)
{
	if (ctrl == controller)
		if (CheckChangeState()) UpdateBody();
}

bool ComponentPhysics::CheckController(ComponentCharacterController* ctrl)
{
	return (ctrl != nullptr && ctrl->game_object_attached == game_object_attached);
}

bool ComponentPhysics::CheckChangeState()
{
	if (!controller && !rigid_body && colliders.empty()) // Delete if not has physic components
	{
		Destroy();
		state = PhysicState::DISABLED;
		return true;
	}

	PhysicState new_state = PhysicState::DISABLED;

	//if (controller && controller->IsEnabled())
	//{
	//	new_state = PhysicState::CTRL_CHARACTER;
	//}
	/*else*/ if (rigid_body  && rigid_body->IsEnabled())
	{
		new_state = PhysicState::DYNAMIC;
	}
	else if (HasEnabledColliders())
	{
		new_state = PhysicState::STATIC;
	}

	if (new_state != state) {
		state = new_state;
		return true;
	}

	return false;
}

void ComponentPhysics::UpdateBody()
{
	if (actor)  // Dettach and Delete Actor
	{
		PxU32 num_shapes = actor->getNbShapes();
		PxShape* shapes = nullptr; // Buffer Shapes 
		actor->getShapes(&shapes, num_shapes);

		for (PxU32 i = 0; i < num_shapes; ++i)
			actor->detachShape(shapes[i]);

		App->physx->RemoveBody(actor);
		actor = nullptr;
	}

	if (state == PhysicState::STATIC || state == PhysicState::DYNAMIC)
	{
		actor = App->physx->CreateBody(transform->GetGlobalMatrix(), IsDynamic());

		if (actor == nullptr) {
			LOG_ENGINE("PhyX Rigid Actor Created at Infinite Transform (Scale x,y,z = 0 ? )");
			state == PhysicState::INVALID_TRANS;
			return;
		}

		for (ComponentCollider* collider : colliders)
			if (collider->enabled && collider->shape) // TODO: check this
				actor->attachShape(*collider->shape);

		if (IsDynamic())
			rigid_body->SetBodyProperties();
	}
}

float3 ComponentPhysics::GetValidPhysicScale()
{
	float3 scale = transform->GetGlobalScale();

	for (int i = 0; i < 3; ++i)
		if (scale[i] == 0.f)
			scale[i] = 0.01f;

	return scale;
}

void ComponentPhysics::GizmoManipulation()
{
	bool is_using_gizmo = ImGuizmo::IsUsing() && game_object_attached->IsSelected();
	PxRigidDynamic* dyn = actor->is<PxRigidDynamic>();
	bool need_sleep_wakeup = IsDynamic() && !IsKinematic();

	if (gizmo_selected)
	{
		if (!is_using_gizmo)
		{
			gizmo_selected = false;
			if (need_sleep_wakeup)
				dyn->wakeUp();
		}
		else
			if (need_sleep_wakeup)
				dyn->putToSleep();
	}
	else
		if (is_using_gizmo)
			gizmo_selected = true;
}

void ComponentPhysics::UpdatePositioning()
{
	if (!Time::IsPlaying() || gizmo_selected)
	{
		PxTransform trans;
		if (!F4X4_TO_PXTRANS(transform->GetGlobalMatrix(), trans)) {
			LOG_ENGINE("Error! GameObject %s transform is NaN or Infinite -> Physics Can't be updated ");
			return;
		}

		if (IsKinematic() && Time::IsPlaying())
			actor->is<PxRigidDynamic>()->setKinematicTarget(trans);
		else
			actor->setGlobalPose(trans);
	}
	else
	{
		if (IsDynamic())
		{
			PxTransform trans = actor->getGlobalPose(); // Get Controller Position
			transform->SetGlobalPosition(PXVEC3_TO_F3(trans.p));
			transform->SetGlobalRotation(PXQUAT_TO_QUAT(trans.q));
		}
		else
		{
			PxTransform trans;
			if (!F4X4_TO_PXTRANS(transform->GetGlobalMatrix(), trans)) {
				LOG_ENGINE("Error! GameObject %s transform is NaN or Infinite -> Physics Can't be updated ");
				return;
			}
			actor->setGlobalPose(trans);
		}
	}
}

void ComponentPhysics::WakeUp()
{
	if (IsDynamic())
	{
		actor->is<PxRigidDynamic>()->wakeUp();
	}
	actor->setGlobalPose(actor->getGlobalPose());
	
}

void ComponentPhysics::PutToSleep()
{
	if (IsDynamic())
		actor->is<PxRigidDynamic>()->putToSleep();
}

void ComponentPhysics::ChangedFilters()
{
	if (IsDisabled()) return;

	//App->physx->px_scene->resetFiltering(*actor);

}


bool ComponentPhysics::HasEnabledColliders()
{
	for (ComponentCollider* collider : colliders)
		if (collider->enabled)
			return true;

	return false;
}

bool ComponentPhysics::ShapeAttached(PxShape* shape)
{
	bool ret = false;
	PxU32 num_shapes = actor->getNbShapes();
	PxShape** shapes = new PxShape * [num_shapes]; // Buffer Shapes 
	actor->getShapes(shapes, num_shapes);

	for (PxU32 i = 0; i < num_shapes; i++)
		if (shapes[i] == shape)
		{
			ret = true;
			break;
		}

	delete[]shapes;
	return ret;
}

bool ComponentPhysics::IsDynamic() { return state == PhysicState::DYNAMIC; }
bool ComponentPhysics::IsKinematic() { return state == PhysicState::DYNAMIC && rigid_body->is_kinematic; }
bool ComponentPhysics::IsDisabled() { return state == PhysicState::DISABLED; }

