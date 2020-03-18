#include "ComponentSphereCollider.h"
#include "ComponentRigidBody.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "imgui/imgui.h"


ComponentSphereCollider::ComponentSphereCollider(GameObject* go) :ComponentCollider(go)
{
	name.assign("Sphere Collider");
	type = ComponentType::SPHERE_COLLIDER;

	// More useful if its called externaly
	Init();
}

void ComponentSphereCollider::SetRadius(float value)
{
	if (value != radius)
	{
		radius = value;
		UpdateShape();
	}
}


void ComponentSphereCollider::CreateDefaultShape()
{
	ComponentMesh* mesh = game_object_attached->GetComponent<ComponentMesh>();

	if (mesh != nullptr)
	{
		center = mesh->local_aabb.CenterPoint();
		radius = mesh->local_aabb.Size().MaxElement() * 0.5f;
	}
	else
	{
		center = float3::zero();
		radius = 0.5f;
	}

	UpdateShape();
}

void ComponentSphereCollider::UpdateShape()
{
	if (shape != nullptr)
	{
		delete shape;
	}

	final_radius = radius * transform->GetGlobalScale().Abs().MaxElement();
	final_center = center.Mul(final_radius);

	shape = new btSphereShape(final_radius);

	if (aux_body)
		aux_body->setCollisionShape(shape);
	if (detector)
		detector->setCollisionShape(shape);

	if (rigid_body != nullptr)  rigid_body->UpdateCollider();
}

void ComponentSphereCollider::DrawSpecificInspector()
{
	float current_radius = radius;
	ImGui::Title("Radius", 1);	ImGui::DragFloat("##radius", &current_radius, 0.1f, 0.01f, FLT_MAX);
	SetRadius(current_radius);
}

void ComponentSphereCollider::Reset()
{
	ComponentCollider::Reset();
	SetRadius(0.5f);
}

void ComponentSphereCollider::SaveComponent(JSONArraypack* to_save)
{
	ComponentCollider::SaveComponent(to_save);
	to_save->SetNumber("Radius", radius);
}

void ComponentSphereCollider::LoadComponent(JSONArraypack* to_load)
{
	ComponentCollider::LoadComponent(to_load);
	SetRadius(to_load->GetNumber("Radius"));
}

