#include "ComponentCanvas.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "glew/include/glew.h"
#include "imgui/imgui.h"
#include "ReturnZ.h"
#include "mmgr/mmgr.h"


ComponentCanvas::ComponentCanvas(GameObject* obj):Component(obj)
{
	width = 160;
	height = 90;

	type = ComponentType::CANVAS;
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

	if (ImGui::CollapsingHeader("Canvas", &not_destroy))
	{
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}

	return true;
}

void ComponentCanvas::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetBoolean("Enabled", enabled);
	to_save->SetNumber("Type", (int)type);
}

void ComponentCanvas::LoadComponent(JSONArraypack* to_load)
{
	enabled = to_load->GetBoolean("Enabled");
}

void ComponentCanvas::Draw()
{
	ComponentTransform* comp_trans = game_object_attached->GetComponent<ComponentTransform>();

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
}
