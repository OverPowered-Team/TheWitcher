#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleRenderer3D.h"
#include "ComponentCollider.h"
#include "ComponentTransform.h"
#include "ComponentRigidBody.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "ReturnZ.h"

ComponentCollider::ComponentCollider(GameObject* go) : Component(go)
{
	// GameObject Components 
	transform = (transform == nullptr) ? game_object_attached->GetComponent<ComponentTransform>() : transform;

	// Default values 
	SetCenter(float3::zero());
}

ComponentCollider::~ComponentCollider()
{
	if (rb)
	{
		rb->RemoveCollider();
	}

	App->physics->RemoveBody(aux_body);

	delete aux_body;
	delete shape;
}

void ComponentCollider::Init()
{

	// Create aux body 
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0.F, nullptr, nullptr);
	aux_body = new btRigidBody(rbInfo);
	aux_body->setUserPointer(game_object_attached);

	// Create shape 

	CreateShape();
	aux_body->setCollisionShape(shape);
	App->physics->AddBody(aux_body);
	// Search Rigid Body 

	ComponentRigidBody* new_rb = game_object_attached->GetComponent<ComponentRigidBody>();

	if (new_rb != nullptr)
	{
		new_rb->AddCollider(this);
	}

	SetIsTrigger(false);
	SetBouncing(0.1f);
	SetFriction(0.5f);
	SetAngularFriction(0.1f);
}


// Colliders Functions --------------------------------

void ComponentCollider::SetCenter(float3 value)
{
	center = value;
	float3 current_scale = transform->GetGlobalScale();
	final_center = center.Mul(current_scale);
}

void ComponentCollider::SetIsTrigger(bool value)
{
	is_trigger = value;
	(is_trigger)
		? aux_body->setCollisionFlags(aux_body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE)
		: aux_body->setCollisionFlags(aux_body->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
}

void ComponentCollider::SetBouncing(const float value)
{
	bouncing = math::Clamp(value, 0.f, 1.f);
	(rb) ? rb->body->setRestitution(bouncing)
		: aux_body->setRestitution(bouncing);
}

void ComponentCollider::SetFriction(const float value)
{
	friction = value;
	(rb) ? rb->body->setFriction(friction)
		: aux_body->setFriction(friction);
}

void ComponentCollider::SetAngularFriction(const float value)
{
	angular_friction = value;
	(rb) ? rb->body->setRollingFriction(angular_friction)
		: aux_body->setRollingFriction(angular_friction);
}

void ComponentCollider::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetBoolean("IsTrigger", is_trigger);
	to_save->SetFloat3("Center", center);
	to_save->SetNumber("Bouncing", bouncing);
	to_save->SetNumber("Friction", friction);
	to_save->SetNumber("AngularFriction", angular_friction);
}

void ComponentCollider::LoadComponent(JSONArraypack* to_load)
{
	SetCenter(to_load->GetFloat3("Center"));
	SetIsTrigger(is_trigger = to_load->GetBoolean("IsTrigger"));
	SetBouncing(bouncing = to_load->GetNumber("Bouncing"));
	SetFriction(friction = to_load->GetNumber("Friction"));
	SetAngularFriction(angular_friction = to_load->GetNumber("AngularFriction"));

	UpdateShape();
}

void ComponentCollider::CreateShape()
{
	if (!WrapMesh()) UpdateShape();
}

void ComponentCollider::Update()
{
	static float3 last_scale = transform->GetGlobalScale();
	float3 current_scale = transform->GetGlobalScale();

	if (!last_scale.Equals(current_scale))
	{
		last_scale = current_scale;
		UpdateShape();
	}

	if (rb == nullptr)
	{
		aux_body->setWorldTransform(ToBtTransform(transform->GetGlobalPosition() + GetWorldCenter(), transform->GetGlobalRotation()));
	}

}

void ComponentCollider::DrawScene()
{
	if (game_object_attached->IsSelected()/* && App->scene->editor_mode*/)
	{
		App->physics->RenderCollider(this);
	}
}

void ComponentCollider::Reset()
{
	SetCenter(float3::zero());
	SetIsTrigger(false);
	SetBouncing(0.1f);
	SetFriction(0.5f);
	SetAngularFriction(0.1f);
}

bool ComponentCollider::DrawInspector()
{
	static bool check;

	check = enabled;

	if (ImGui::Checkbox("##CmpActive", &check)) {
		ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
		enabled = check;
	}

	float3 current_center = center;
	bool current_is_trigger = is_trigger;
	float current_bouncing = bouncing;
	float current_friction = friction;
	float current_angular_friction = angular_friction;

	ImGui::SameLine();

	if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Spacing();
		ImGui::Title("Center", 1);			if (ImGui::DragFloat3("##center", current_center.ptr(), 0.1f)) { SetCenter(current_center); }

		DrawSpecificInspector();

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Title("Is Trigger", 1);		if (ImGui::Checkbox("##is_trigger", &current_is_trigger)) { SetIsTrigger(current_is_trigger); }
		ImGui::Spacing();
		ImGui::Title("Physic Material", 1); ImGui::Text("");
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Title("Bouncing", 2);	    if (ImGui::DragFloat("##bouncing", &current_bouncing, 0.01f, 0.00f, 1.f)) { SetBouncing(current_bouncing); }
		ImGui::Title("Linear Fric.", 2);	if (ImGui::DragFloat("##friction", &current_friction, 0.01f, 0.00f, FLT_MAX)) { SetFriction(current_friction); }
		ImGui::Title("Angular Fric.", 2);	if (ImGui::DragFloat("##angular_friction", &current_angular_friction, 0.01f, 0.00f, FLT_MAX)) { SetAngularFriction(current_angular_friction); }
		ImGui::Spacing();
	}

	return true;
}

float3 ComponentCollider::GetWorldCenter()
{
	return transform->GetGlobalRotation().Mul(final_center);
}

