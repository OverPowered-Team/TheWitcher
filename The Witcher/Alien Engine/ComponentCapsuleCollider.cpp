#include "Application.h"
#include "ComponentPhysics.h"
#include "ComponentCapsuleCollider.h"
#include "ComponentTransform.h"
#include "GameObject.h"
#include "imgui/imgui.h"

ComponentCapsuleCollider::ComponentCapsuleCollider(GameObject* go) : ComponentCollider(go)
{
	name.assign("Capsule Collider");
	type = ComponentType::CAPSULE_COLLIDER;
	shape = App->physx->CreateShape(PxCapsuleGeometry( .5f, .5f), *material);
	SetOrientation(Orientation::Y);
	InitCollider();
}

void ComponentCapsuleCollider::SetRotation(const float3& value)
{
	if (rotation.Equals(value)) return;
	rotation = value;
	PxTransform trans = shape->getLocalPose();
	float3 rad_rotation = DEGTORAD * GetOrientationRotation();
	trans.q = QUAT_TO_PXQUAT(Quat::FromEulerXYZ(rad_rotation.x, rad_rotation.y, rad_rotation.z));
	BeginUpdateShape();
	shape->setLocalPose(trans);
	EndUpdateShape();
}

void ComponentCapsuleCollider::SetRadius(float value)
{
	if (value == radius) return;
	radius = value;
	PxCapsuleGeometry geo(radius , height * 0.5f);
	BeginUpdateShape();
	shape->setGeometry(geo);
	EndUpdateShape();
}

void ComponentCapsuleCollider::SetHeight(float value)
{
	if (value == height) return;
	height = value;
	PxCapsuleGeometry geo(radius, height * 0.5f);
	BeginUpdateShape();
	shape->setGeometry(geo);
	EndUpdateShape();
}

void ComponentCapsuleCollider::SetOrientation(Orientation value)
{
	if (value == orientation) return;
	orientation = value;
	PxTransform trans = shape->getLocalPose();
	float3 rad_rotation = DEGTORAD * GetOrientationRotation();
	trans.q = QUAT_TO_PXQUAT(Quat::FromEulerXYZ(rad_rotation.x, rad_rotation.y, rad_rotation.z));
	BeginUpdateShape();
	shape->setLocalPose(trans);
	EndUpdateShape();
}

void ComponentCapsuleCollider::DrawSpecificInspector()
{
	float current_radius = radius;
	float current_height = height;

	ImGui::Title("Radius", 1);	if (ImGui::DragFloat("##radius", &current_radius, 0.1f, 0.01f, FLT_MAX)) { SetRadius(current_radius); }
	ImGui::Title("Height", 1);	if (ImGui::DragFloat("##height", &current_height, 0.1f, 0.01f, FLT_MAX)) { SetHeight(current_height); }
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

				if (current_item == "X")		SetOrientation(Orientation::X);
				else if (current_item == "Y")	SetOrientation(Orientation::Y);
				else if (current_item == "Z")	SetOrientation(Orientation::Z);
			}

			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}

float3 ComponentCapsuleCollider::GetOrientationRotation()
{
	float3 final_rot = rotation;

	switch (orientation)
	{
	case ComponentCapsuleCollider::Orientation::Y:
		final_rot.z += 90.f;
		break;
	case ComponentCapsuleCollider::Orientation::Z:
		final_rot.y += 90.f;
		break;
	}

	return final_rot;
}

void ComponentCapsuleCollider::Reset()
{
}

void ComponentCapsuleCollider::Clone(Component* clone)
{
	// TODO: REWORK this clone
	ComponentCapsuleCollider* capsule_clone = (ComponentCapsuleCollider*)clone;
	capsule_clone->SetHeight(height);
	capsule_clone->SetRotation(rotation);
	capsule_clone->SetRadius(radius);
	capsule_clone->SetCenter(center);
}

void ComponentCapsuleCollider::SaveComponent(JSONArraypack* to_save)
{
	ComponentCollider::SaveComponent(to_save);
	to_save->SetNumber("Radius", radius);
	to_save->SetNumber("Height", height);
	to_save->SetNumber("Orientation", (int)orientation);
}

void ComponentCapsuleCollider::LoadComponent(JSONArraypack* to_load)
{
	ComponentCollider::LoadComponent(to_load);
	SetRadius(to_load->GetNumber("Radius"));
	SetHeight(to_load->GetNumber("Height"));
	orientation = (Orientation)(int)to_load->GetNumber("Orientation");
}