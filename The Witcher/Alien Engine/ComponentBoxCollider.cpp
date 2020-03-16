#include "ComponentBoxCollider.h"
#include "ComponentRigidBody.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ModulePhysics.h"
#include "GameObject.h"
#include "imgui/imgui.h"

ComponentBoxCollider::ComponentBoxCollider(GameObject* go) : ComponentCollider(go)
{
	name.assign("Box Collider");
	type = ComponentType::BOX_COLLIDER;

	// More useful if its called externaly
	Init();
}

void ComponentBoxCollider::SetCenter(float3 value)
{
	center = value;
	final_center = center.Mul(final_size);
}

void ComponentBoxCollider::SetSize(float3 value)
{
	if (!value.Equals(size))
	{
		size = value;
		UpdateShape();
	}
}

void ComponentBoxCollider::DrawSpecificInspector()
{
	float3 current_size = size;

	ImGui::Title("Size", 1);			ImGui::DragFloat3("##size", current_size.ptr(), 0.1f, 0.01f, FLT_MAX);

	SetSize(current_size);
}

//float3 t = { transform->global_transformation[0][0], transform->global_transformation[1][1], transform->global_transformation[2][2] };
//float3 final_size = CheckInvalidCollider(size.Mul(t)) * 0.5f;

void ComponentBoxCollider::CreateDefaultShape()
{
	ComponentMesh* mesh = game_object_attached->GetComponent<ComponentMesh>();

	if (mesh != nullptr)
	{
		center = mesh->local_aabb.CenterPoint();
		size = mesh->local_aabb.Size();
	}
	else
	{
		size = float3::one();
		center = float3::zero();
	}

	UpdateShape();
}

void ComponentBoxCollider::UpdateShape()
{
	if (shape == nullptr)
	{
		delete shape;
	}

	final_size = size.Mul(transform->GetGlobalScale());
	final_center = center.Mul(final_size);

	shape = new btBoxShape(ToBtVector3(final_size.Abs() * 0.5f));

	if (aux_body) 
		aux_body->setCollisionShape(shape);
	if (detector) 
		detector->setCollisionShape(shape);

	if (rigid_body != nullptr)  rigid_body->UpdateCollider();
}

void ComponentBoxCollider::Clone(Component* clone)
{
	ComponentBoxCollider* box_clone = (ComponentBoxCollider*)clone;
	center = box_clone->GetCenter();
	size = box_clone->GetSize();
	UpdateShape();
}

void ComponentBoxCollider::Reset()
{
	ComponentCollider::Reset();
	SetSize(float3::one());
}

void ComponentBoxCollider::SaveComponent(JSONArraypack* to_save)
{
	ComponentCollider::SaveComponent(to_save);
	to_save->SetFloat3("Size", size);
}

void ComponentBoxCollider::LoadComponent(JSONArraypack* to_load)
{
	ComponentCollider::LoadComponent(to_load);
	SetSize(to_load->GetFloat3("Size"));
}

float3 ComponentBoxCollider::CheckInvalidCollider(float3 size)
{
	return size.Max(float3(0.01, 0.01, 0.01));
}
