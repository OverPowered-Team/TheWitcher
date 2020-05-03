#include "ComponentLightSpot.h"
#include "glew/include/glew.h"
#include "GameObject.h"
#include "imgui/imgui.h"
#include "ComponentTransform.h"
#include "ModuleResources.h"
#include "ModuleRenderer3D.h"
#include "Application.h"
#include "ReturnZ.h"
#include "ComponentMesh.h"
#include "Gizmos.h"
#include "mmgr/mmgr.h"

#include "Optick/include/optick.h"

ComponentLightSpot::ComponentLightSpot(GameObject* attach) : Component(attach)
{
	type = ComponentType::LIGHT_SPOT;
	App->objects->spot_light_properites.push_back(&light_props);
	App->objects->AddNumOfSpotLights();

#ifndef GAME_VERSION
	bulb = new ComponentMesh(game_object_attached);
	bulb->mesh = App->resources->light_mesh;
#endif
}

ComponentLightSpot::~ComponentLightSpot()
{
#ifndef GAME_VERSION
	delete bulb;
#endif

	App->objects->spot_light_properites.remove(&light_props);
	App->objects->ReduceNumOfSpotLights();
}

void ComponentLightSpot::Update()
{
	OPTICK_EVENT();

	LightLogic();
}

void ComponentLightSpot::DrawScene(ComponentCamera* camera)
{
	OPTICK_EVENT(); 

	if (this->game_object_attached->IsSelected())
	{
		App->renderer3D->RenderCircleAroundZ(light_props.position.x, light_props.position.y, light_props.position.z, light_props.intensity * RADIUS_INTENSITY_MULTIPLIER_SPOT);
	}
	else if (IsEnabled())
	{
		App->renderer3D->RenderCircleAroundZ(light_props.position.x, light_props.position.y, light_props.position.z, light_props.intensity * RADIUS_INTENSITY_MULTIPLIER_SPOT, 0.1f);
	}
}

void ComponentLightSpot::LightLogic()
{
	OPTICK_EVENT();

	ComponentTransform* transform = (ComponentTransform*)game_object_attached->GetComponent(ComponentType::TRANSFORM);
	light_props.position = float3(transform->GetGlobalPosition().x, transform->GetGlobalPosition().y, transform->GetGlobalPosition().z);
	light_props.direction = game_object_attached->transform->GetGlobalRotation().WorldZ();
}

bool ComponentLightSpot::DrawInspector()
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

	if (ImGui::CollapsingHeader("Light Spot", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		RightClickMenu("Light Spot");

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
		ImGui::DragFloat("Constant", &light_props.constant, 0.01f, 0.0f);
		ImGui::DragFloat("Linear", &light_props.linear, 0.01f, 0.0f);
		ImGui::DragFloat("Quadratic", &light_props.quadratic, 0.01f, 0.0f);
		ImGui::DragFloat("Cut Off", &light_props.cut_off, 0.10f);
		ImGui::DragFloat("Outer Cut Off", &light_props.outer_cut_off, 0.10f);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}
	else
		RightClickMenu("Light Spot");

	return true;
}

void ComponentLightSpot::OnDisable()
{

}

void ComponentLightSpot::Clone(Component* clone)
{
	clone->enabled = enabled;
	clone->not_destroy = not_destroy;
	ComponentLightSpot* light = (ComponentLightSpot*)clone;
	light->renderer_id = renderer_id;
	light->print_icon = print_icon;
}

void ComponentLightSpot::Reset()
{
	print_icon = true;
}

void ComponentLightSpot::SetComponent(Component* component)
{
	if (component->GetType() == type) {

		ComponentLightSpot* light = (ComponentLightSpot*)component;

		renderer_id = light->renderer_id;
		print_icon = light->print_icon;
	}
}

void ComponentLightSpot::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetBoolean("Enabled", enabled);
	to_save->SetString("ID", std::to_string(ID).data());
	to_save->SetBoolean("PrintIcon", print_icon);

	to_save->SetNumber("Intensity", float(light_props.intensity));
	to_save->SetFloat3("Position", float3(light_props.position));
	to_save->SetFloat3("Direction", float3(light_props.direction));
	to_save->SetFloat3("Ambient", float3(light_props.ambient));
	to_save->SetFloat3("Diffuse", float3(light_props.diffuse));
	to_save->SetFloat3("Specular", float3(light_props.specular));
	to_save->SetNumber("Constant", double(light_props.constant));
	to_save->SetNumber("Linear", double(light_props.linear));
	to_save->SetNumber("Quadratic", double(light_props.quadratic));
	to_save->SetNumber("Cut_Off", double(light_props.cut_off));
	to_save->SetNumber("Outer_Cut_Off", double(light_props.outer_cut_off));
}

void ComponentLightSpot::LoadComponent(JSONArraypack* to_load)
{
	enabled = to_load->GetBoolean("Enabled");
	ID = std::stoull(to_load->GetString("ID"));
	print_icon = to_load->GetBoolean("PrintIcon");

	light_props.intensity = (float)to_load->GetNumber("Intensity");
	light_props.position = to_load->GetFloat3("Position");
	light_props.direction = to_load->GetFloat3("Direction");
	light_props.ambient = to_load->GetFloat3("Ambient");
	light_props.diffuse = to_load->GetFloat3("Diffuse");
	light_props.specular = to_load->GetFloat3("Specular");
	light_props.constant = (float)to_load->GetNumber("Constant");
	light_props.linear = (float)to_load->GetNumber("Linear");
	light_props.quadratic = (float)to_load->GetNumber("Quadratic");
	light_props.cut_off = (float)to_load->GetNumber("Cut_Off");
	light_props.outer_cut_off = (float)to_load->GetNumber("Outer_Cut_Off");
}

void ComponentLightSpot::DrawIconLight()
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