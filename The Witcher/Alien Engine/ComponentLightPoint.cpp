#include "ComponentLightPoint.h"
#include "glew/include/glew.h"
#include "GameObject.h"
#include "imgui/imgui.h"
#include "ComponentTransform.h"
#include "Application.h"
#include "ReturnZ.h"
#include "ComponentMesh.h"
#include "ModuleResources.h"
#include "ModuleRenderer3D.h"
#include "Gizmos.h"
#include "mmgr/mmgr.h"

#include "Optick/include/optick.h"

ComponentLightPoint::ComponentLightPoint(GameObject* attach) : Component(attach)
{
	type = ComponentType::LIGHT_POINT;
	App->objects->point_light_properites.push_back(&light_props);
	App->objects->AddNumOfPointLights();

#ifndef GAME_VERSION
	bulb = new ComponentMesh(game_object_attached);
	bulb->mesh = App->resources->light_mesh;
#endif
}

ComponentLightPoint::~ComponentLightPoint()
{
#ifndef GAME_VERSION
	delete bulb;
#endif

	App->objects->point_light_properites.remove(&light_props);

	App->objects->ReduceNumOfPointLights();
}

void ComponentLightPoint::LightLogic()
{
	OPTICK_EVENT();
	light_props.position = float3(game_object_attached->transform->GetGlobalPosition().x, game_object_attached->transform->GetGlobalPosition().y, game_object_attached->transform->GetGlobalPosition().z);
}

void ComponentLightPoint::Update()
{
	OPTICK_EVENT();

	LightLogic();
}

void ComponentLightPoint::DrawScene(ComponentCamera* camera)
{
	OPTICK_EVENT();

	if (this->game_object_attached->IsSelected())
	{
		App->renderer3D->RenderCircleAroundZ(light_props.position.x, light_props.position.y, light_props.position.z, light_props.intensity * RADIUS_INTENSITY_MULTIPLIE_POINT);
		App->renderer3D->RenderCircleAroundX(light_props.position.x, light_props.position.y, light_props.position.z, light_props.intensity * RADIUS_INTENSITY_MULTIPLIE_POINT);
	}
	else if (IsEnabled())
	{
		DrawIconLight();

		App->renderer3D->RenderCircleAroundZ(light_props.position.x, light_props.position.y, light_props.position.z, light_props.intensity * RADIUS_INTENSITY_MULTIPLIE_POINT, 0.1f);
		App->renderer3D->RenderCircleAroundX(light_props.position.x, light_props.position.y, light_props.position.z, light_props.intensity * RADIUS_INTENSITY_MULTIPLIE_POINT, 0.1f);
	}
}

bool ComponentLightPoint::DrawInspector()
{
	static bool en;
	ImGui::PushID(this);
	en = enabled;
	if (ImGui::Checkbox("##CmpActive", &en)) {
		ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
		enabled = en;
		if (!enabled)
			OnDisable();
		else
			OnEnable();
	}
	ImGui::PopID();
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Light Point", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		RightClickMenu("Light Point");

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::PushID("printiconlight");
		ImGui::Checkbox("Print Icon", &print_icon);
		ImGui::PopID();

		// Parameters ---------
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Settings:");
		ImGui::DragFloat("Intensity", &light_props.intensity, 0.01f, 0.0f, 2.0f);
		ImGui::ColorEdit3("Ambient", light_props.ambient.ptr());
		ImGui::ColorEdit3("Diffuse", light_props.diffuse.ptr());
		ImGui::ColorEdit3("Specular", light_props.specular.ptr());
		ImGui::DragFloat("Constant", &light_props.constant, 0.10f, 0.0f, 1.0f);
		ImGui::DragFloat("Linear", &light_props.linear, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Quadratic", &light_props.quadratic, 0.01f, 0.0f, 2.0f);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}
	else
		RightClickMenu("Light Point");

	return true;
}

void ComponentLightPoint::OnDisable()
{

}

void ComponentLightPoint::Clone(Component* clone)
{
	clone->enabled = enabled;
	clone->not_destroy = not_destroy;
	ComponentLightPoint* light = (ComponentLightPoint*)clone;
	light->renderer_id = renderer_id;
	light->print_icon = print_icon;
}

void ComponentLightPoint::Reset()
{
	print_icon = true;
}

void ComponentLightPoint::SetComponent(Component* component)
{
	if (component->GetType() == type) {

		ComponentLightPoint* light = (ComponentLightPoint*)component;

		renderer_id = light->renderer_id;
		print_icon = light->print_icon;
	}
}

void ComponentLightPoint::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetBoolean("Enabled", enabled);
	to_save->SetString("ID", std::to_string(ID).data());
	to_save->SetBoolean("PrintIcon", print_icon);

	to_save->SetNumber("Intensity", float(light_props.intensity));
	to_save->SetFloat3("Position", float3(light_props.position));
	to_save->SetFloat3("Ambient", float3(light_props.ambient));
	to_save->SetFloat3("Diffuse", float3(light_props.diffuse));
	to_save->SetFloat3("Specular", float3(light_props.specular));
	to_save->SetNumber("Constant", float(light_props.constant));
	to_save->SetNumber("Linear", float(light_props.linear));
	to_save->SetNumber("Quadratic", float(light_props.quadratic));
}

void ComponentLightPoint::LoadComponent(JSONArraypack* to_load)
{
	enabled = to_load->GetBoolean("Enabled");
	ID = std::stoull(to_load->GetString("ID"));
	print_icon = to_load->GetBoolean("PrintIcon");

	light_props.intensity = (float)to_load->GetNumber("Intensity");
	light_props.ambient = to_load->GetFloat3("Ambient");
	light_props.diffuse = to_load->GetFloat3("Diffuse");
	light_props.specular = to_load->GetFloat3("Specular");
	light_props.constant = (float)to_load->GetNumber("Constant");
	light_props.linear = (float)to_load->GetNumber("Linear");
	light_props.quadratic = (float)to_load->GetNumber("Quadratic");
}

void ComponentLightPoint::DrawIconLight()
{
	OPTICK_EVENT();

	if (bulb != nullptr && print_icon)
	{
		ComponentTransform* transform = (ComponentTransform*)game_object_attached->GetComponent(ComponentType::TRANSFORM);
		float3 pos = transform->GetGlobalPosition();
		float4x4 matrix = float4x4::FromTRS({ pos.x - 0.133f, pos.y, pos.z }, transform->GetGlobalRotation(), { 0.2f, 0.18f, 0.2f });
		glDisable(GL_LIGHTING);
		Gizmos::DrawPoly(bulb->mesh, matrix, Color(0.0f, 255.0f, 0.0f));
		glEnable(GL_LIGHTING);
	}
}