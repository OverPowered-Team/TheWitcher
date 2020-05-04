#include "ComponentCanvas.h"
#include "GameObject.h"
#include "ComponentUI.h"
#include "ComponentTransform.h"
#include "ResourceShader.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include "glew/include/glew.h"
#include "imgui/imgui.h"
#include "ReturnZ.h"
#include "mmgr/mmgr.h"
#include "ComponentCamera.h"
#include "ModuleObjects.h"
#include "Viewport.h"

#include "Optick/include/optick.h"

ComponentCanvas::ComponentCanvas(GameObject* obj):Component(obj)
{
	width = 160;
	height = 90;

	type = ComponentType::CANVAS;
}

ComponentCanvas::~ComponentCanvas()
{
	/*text_shader->DecreaseReferences();
	text_shader = nullptr;
	text_ortho->DecreaseReferences();
	text_ortho = nullptr;*/
}

void ComponentCanvas::DrawScene(ComponentCamera* camera)
{
	Draw();
}

bool ComponentCanvas::DrawInspector()
{
	static bool check;

	ImGui::PushID(this);
	check = enabled;
	if (ImGui::Checkbox("##CmpActive", &check)) {
		ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
		enabled = check;
	}
	ImGui::PopID();
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Canvas", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Spacing();

		ImGui::Text("Canvas World");

		ImGui::SameLine();
		ImGui::Checkbox("##isWorld", &isWorld);
		ImGui::Spacing(); ImGui::Spacing();

		if (isWorld)
		{
			ImGui::Text("BillBoard Type");
			ImGui::SameLine();
			static int type = 0;
			ImGui::Combo("##BillBoardType", &type, "Screen\0World\0Axis\0");
			switch (BillboardType(type))
			{
			case BillboardType::SCREEN: {
				bbtype = BillboardType::SCREEN;
				break; }
			case BillboardType::WORLD: {
				bbtype = BillboardType::WORLD;
				break; }
			case BillboardType::AXIS: {
				bbtype = BillboardType::AXIS;
				break; }
			default: {
				break; }
			}


			ImGui::Spacing(); ImGui::Spacing();
		}

		ImGui::Separator();
		ImGui::Spacing();
	}

	return true;
}

void ComponentCanvas::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetBoolean("Enabled", enabled);
	to_save->SetBoolean("isWorld", isWorld);
	to_save->SetNumber("Type", (int)type);
	to_save->SetNumber("BBType", (int)bbtype);
	
}

void ComponentCanvas::LoadComponent(JSONArraypack* to_load)
{
	enabled = to_load->GetBoolean("Enabled");
	isWorld = to_load->GetBoolean("isWorld");
	bbtype  = (BillboardType)(int)to_load->GetNumber("BBType");
}

void ComponentCanvas::Draw()
{
	OPTICK_EVENT();
#ifndef GAME_VERSION
	if (!App->objects->printing_scene)
		return;

	ComponentTransform* comp_trans = game_object_attached->GetComponent<ComponentTransform>();
	glDisable(GL_LIGHTING);
	glBegin(GL_LINE_LOOP);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	float3 pos = comp_trans->GetGlobalPosition();

	float3 v1 = float3(pos.x - width * 0.5F, pos.y + height * 0.5F, pos.z);
	float3 v2 = float3(pos.x + width * 0.5F, pos.y + height * 0.5F, pos.z);
	float3 v3 = float3(pos.x + width * 0.5F, pos.y - height * 0.5F, pos.z);
	float3 v4 = float3(pos.x - width * 0.5F, pos.y - height * 0.5F, pos.z);

	glVertex3f(v1.x, v1.y, v1.z);
	glVertex3f(v2.x, v2.y, v2.z);
	glVertex3f(v3.x, v3.y, v3.z);
	glVertex3f(v4.x, v4.y, v4.z);

	glEnd();
	glEnable(GL_LIGHTING);
#endif

}

float3 ComponentCanvas::GetWorldPositionInCanvas(const float3& world_position)
{
	float2 position = ComponentCamera::WorldToScreenPoint(world_position);

	return float3(
		game_object_attached->transform->GetGlobalPosition().x + (position.x * width / App->objects->game_viewport->GetSize().x),
		game_object_attached->transform->GetGlobalPosition().y + (position.y * height / App->objects->game_viewport->GetSize().y),
		game_object_attached->transform->GetGlobalPosition().z
		);
}


