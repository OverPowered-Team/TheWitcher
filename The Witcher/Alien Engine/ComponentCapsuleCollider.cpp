#include "ComponentCapsuleCollider.h"
#include "ComponentTransform.h"
#include "ComponentRigidBody.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "imgui/imgui.h"

ComponentCapsuleCollider::ComponentCapsuleCollider(GameObject* go) : ComponentCollider(go)
{
	name.assign("Capsule Collider");
	type = ComponentType::CAPSULE_COLLIDER;
	capsule_type = CapsuleType::Y;

	// More useful if its called externaly
	Init();
}

void ComponentCapsuleCollider::SetRadius(float value)
{
	if (radius != value)
	{
		radius = value;
		UpdateShape();
	}
}

void ComponentCapsuleCollider::SetHeight(float value)
{
	if (height != value)
	{
		height = value;
		UpdateShape();
	}
}

void ComponentCapsuleCollider::DrawSpecificInspector()
{
	ImGui::Title("Radius", 1);	if (ImGui::DragFloat("##radius", &radius, 0.1f, 0.01f, FLT_MAX)) { UpdateShape(); }
	ImGui::Title("Height", 1);	if (ImGui::DragFloat("##height", &height, 0.1f, 0.01f, FLT_MAX)) { UpdateShape(); }
	ImGui::Title("Direction");

	static const char* items[] = { "X", "Y", "Z" };
	static const char* current_item = items[1];

	if (ImGui::BeginComboEx(std::string("##viewport_modes").c_str(), std::string(" " + std::string(current_item)).c_str(), 200, ImGuiComboFlags_NoArrowButton))
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); ++n)
		{
			bool is_selected = (current_item == items[n]);

			if (ImGui::Selectable(std::string("   " + std::string(items[n])).c_str(), is_selected))
			{
				current_item = items[n];

				if (current_item == "X")				capsule_type = CapsuleType::X;
				else if (current_item == "Y")			capsule_type = CapsuleType::Y;
				else if (current_item == "Z")			capsule_type = CapsuleType::Z;

				UpdateShape();
			}

			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}
}

void ComponentCapsuleCollider::Reset()
{
}

void ComponentCapsuleCollider::SaveComponent(JSONArraypack* to_save)
{
	ComponentCollider::SaveComponent(to_save);
	to_save->SetNumber("Radius", radius);
	to_save->SetNumber("Height", height);
	to_save->SetNumber("CapsuleType", (int)capsule_type);
}

void ComponentCapsuleCollider::LoadComponent(JSONArraypack* to_load)
{
	ComponentCollider::LoadComponent(to_load);

	radius = to_load->GetNumber("Radius");
	height = to_load->GetNumber("Height");
	capsule_type = (CapsuleType)(int)to_load->GetNumber("CapsuleType");

	UpdateShape();
}

void ComponentCapsuleCollider::CreateDefaultShape()
{
	ComponentMesh* mesh = game_object_attached->GetComponent<ComponentMesh>();

	if (mesh != nullptr)
	{
		float3 scale = transform->GetGlobalScale();
		float3 size = mesh->local_aabb.Size();

		switch (capsule_type)
		{
		case ComponentCapsuleCollider::CapsuleType::X:
			height = size.x;
			radius = math::Max(size.z, size.y) * 0.5f;
			break;
		case ComponentCapsuleCollider::CapsuleType::Y:
			height = size.y;
			radius = math::Max(size.z, size.x) * 0.5f;
			break;
		case ComponentCapsuleCollider::CapsuleType::Z:
			height = size.z;
			radius = math::Max(size.y, size.x) * 0.5f;
			break;
		}

		center = mesh->local_aabb.CenterPoint();
	}
	else
	{
		center = float3::zero();
		height = 1.f;
		radius = 0.5f;
	}

	UpdateShape();
}

void ComponentCapsuleCollider::UpdateShape()
{
	if (shape != nullptr)
	{
		delete shape;
	}

	float3 scale = transform->GetGlobalScale();

	final_height = height;
	final_radius = radius;
	final_center = center;

	switch (capsule_type)
	{
	case ComponentCapsuleCollider::CapsuleType::X:
		final_height *= scale.x;
		final_radius *= math::Max(scale.z, scale.y);
		break;
	case ComponentCapsuleCollider::CapsuleType::Y:
		final_height *= scale.y;
		final_radius *= math::Max(scale.z, scale.x);
		break;
	case ComponentCapsuleCollider::CapsuleType::Z:
		final_height *= scale.z;
		final_radius *= math::Max(scale.x, scale.y);
		break;
	}

	switch (capsule_type)
	{
	case ComponentCapsuleCollider::CapsuleType::X:
		shape = new btCapsuleShapeX(final_radius, final_height);
		break;
	case ComponentCapsuleCollider::CapsuleType::Y:
		shape = new btCapsuleShape(final_radius, final_height);
		break;
	case ComponentCapsuleCollider::CapsuleType::Z:
		shape = new btCapsuleShapeZ(final_radius, final_height);
		break;
	}

	if (aux_body)
		aux_body->setCollisionShape(shape);
	if (detector)
		detector->setCollisionShape(shape);

	if (rigid_body != nullptr)  rigid_body->UpdateCollider();
}



