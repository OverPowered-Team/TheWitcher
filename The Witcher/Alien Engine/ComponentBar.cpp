#include "Application.h"
#include "ModuleUI.h"
#include "PanelProject.h"
#include "PanelGame.h"
#include "ComponentCamera.h"
#include "ComponentImage.h"
#include "ComponentTransform.h"
#include "ComponentBar.h"
#include "ResourceTexture.h"
#include "ModuleResources.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ReturnZ.h"
#include "imgui/imgui.h"
#include "mmgr/mmgr.h"

ComponentBar::ComponentBar(GameObject* obj):ComponentUI(obj)
{
	ui_type = ComponentType::UI_BAR;
	tabbable = false;
}

bool ComponentBar::DrawInspector()
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

	if (ImGui::CollapsingHeader("Bar", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		RightClickMenu("Bar");

		ImGui::Spacing();

		ImGui::Text("Min Value: "); ImGui::SameLine(150);
		if (ImGui::DragFloat("##MinValue", &minValue, 0.5F, 0, 0, "%.1f", 1, game_object_attached->is_static))
		{
			CalculateFactor();
		}

		ImGui::Text("Max value:	"); ImGui::SameLine(150);
		if (ImGui::DragFloat("##MaxValue", &maxValue, 0.5F, 0, 0, "%.1f", 1, game_object_attached->is_static))
		{
			CalculateFactor();
		}

		ImGui::Text("Current value:	"); ImGui::SameLine(150);
		if (ImGui::DragFloat("##CurrentValue", &currentValue, 0.5F, minValue, maxValue, "%.1f", 1, game_object_attached->is_static))
		{
			CalculateFactor();
		}
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::PushID(this);
		ImGui::Text("Offset:	"); ImGui::SameLine(150); ImGui::SetNextItemWidth(100);
		ImGui::DragFloat("X", &offsetX, 0.1F, 0, 0, "%.1f", 1, game_object_attached->is_static);
		ImGui::SameLine(); ImGui::SetNextItemWidth(100);
		ImGui::DragFloat("Y", &barScaleY, 0.1F, 0, 0, "%.1f", 1, game_object_attached->is_static);
		ImGui::PopID();

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text("Background Texture");

		ImGui::SameLine(150);
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.16f, 0.29F, 0.5, 1 });
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.16f, 0.29F, 0.5, 1 });
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.16f, 0.29F, 0.5, 1 });
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);

		ImGui::Button((texture == nullptr) ? "NULL" : std::string(texture->GetName()).data(), { ImGui::GetWindowWidth() * 0.55F , 0 });

		if (ImGui::IsItemClicked() && texture != nullptr) {
			App->ui->panel_project->SelectFile(texture->GetAssetsPath(), App->resources->assets);
		}

		ImGui::PopStyleColor(3);
		if (ImGui::BeginDragDropTarget()) {
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_PROJECT_NODE, ImGuiDragDropFlags_SourceNoDisableHover);
			if (payload != nullptr && payload->IsDataType(DROP_ID_PROJECT_NODE)) {
				FileNode* node = *(FileNode**)payload->Data;
				if (node != nullptr && node->type == FileDropType::TEXTURE) {
					std::string path = App->file_system->GetPathWithoutExtension(node->path + node->name);
					path += "_meta.alien";
					u64 ID = App->resources->GetIDFromAlienPath(path.data());
					if (ID != 0) {
						ResourceTexture* texture = (ResourceTexture*)App->resources->GetResourceWithID(ID);
						if (texture != nullptr) {
							ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
							SetTexture(texture);
						}
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
		if (texture != nullptr) {
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
			if (ImGui::Button("X") && texture != nullptr) {
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
				ClearTexture();
			}
			ImGui::PopStyleColor(3);
		}

		/*----------BAR TEXTURE------------------*/
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text("Bar Texture");

		ImGui::SameLine(150);
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.16f, 0.29F, 0.5, 1 });
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.16f, 0.29F, 0.5, 1 });
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.16f, 0.29F, 0.5, 1 });
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);

		ImGui::Button((barTexture == nullptr) ? "NULL" : std::string(barTexture->GetName()).data(), { ImGui::GetWindowWidth() * 0.55F , 0 });

		if (ImGui::IsItemClicked() && barTexture != nullptr) {
			App->ui->panel_project->SelectFile(barTexture->GetAssetsPath(), App->resources->assets);
		}

		ImGui::PopStyleColor(3);
		if (ImGui::BeginDragDropTarget()) {
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_PROJECT_NODE, ImGuiDragDropFlags_SourceNoDisableHover);
			if (payload != nullptr && payload->IsDataType(DROP_ID_PROJECT_NODE)) {
				FileNode* node = *(FileNode**)payload->Data;
				if (node != nullptr && node->type == FileDropType::TEXTURE) {
					std::string path = App->file_system->GetPathWithoutExtension(node->path + node->name);
					path += "_meta.alien";
					u64 ID = App->resources->GetIDFromAlienPath(path.data());
					if (ID != 0) {
						ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(ID);
						if (tex != nullptr) {
							ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
							SetTextureBar(tex);
						}
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
		if (barTexture != nullptr) {
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
			if (ImGui::Button("X##BarTextureID") && barTexture != nullptr) {
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
				if (barTexture != nullptr) {
					barTexture->DecreaseReferences();
					barTexture = nullptr;
				}
			}
			ImGui::PopStyleColor(3);
		}
		/*----------BAR TEXTURE------------------*/

		ImGui::Spacing();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

		ImGui::Text("Background Color");
		ImGui::SameLine(150);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		static bool set_bg_Z = true;
		static Color bg_col;
		bg_col = current_color;
		if (ImGui::ColorEdit4("##Color", &bg_col, ImGuiColorEditFlags_Float)) {
			if (set_bg_Z)
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			set_bg_Z = false;
			current_color = bg_col;
		}
		else if (!set_bg_Z && ImGui::IsMouseReleased(0)) {
			set_bg_Z = true;
		}

		ImGui::Text("Bar Color");
		ImGui::SameLine(150);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		static bool set_bar_Z = true;
		static Color bar_col;
		bar_col = bar_color;
		if (ImGui::ColorEdit4("##BarColor", &bar_col, ImGuiColorEditFlags_Float)) {
			if (set_bar_Z)
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			set_bar_Z = false;
			bar_color = bar_col;
		}
		else if (!set_bar_Z && ImGui::IsMouseReleased(0)) {
			set_bar_Z = true;
		}

		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

		ImGui::PushID("ScaleBar");
		ImGui::Text("Slider Scale:	"); ImGui::SameLine(150); ImGui::SetNextItemWidth(100);
		ImGui::DragFloat("X", &barScaleX, 0.05F); 
		ImGui::SameLine(); ImGui::SetNextItemWidth(100);
		ImGui::DragFloat("Y", &barScaleY, 0.05F);
		ImGui::PopID();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}
	else {
		RightClickMenu("Slider");
	}

	return true;
}

void ComponentBar::Draw(bool isGame)
{
	if (canvas == nullptr || canvas_trans == nullptr) {
		return;
	}
	ComponentTransform* transform = (ComponentTransform*)game_object_attached->GetComponent(ComponentType::TRANSFORM);
	float4x4 matrix = transform->global_transformation;
	transform->global_transformation[0][0] = transform->global_transformation[0][0] * factor * barScaleX; //w
	transform->global_transformation[1][1] = transform->global_transformation[1][1] * barScaleY; //h
	transform->global_transformation[0][3] = matrix[0][3] - matrix[0][0] + transform->global_transformation[0][0] /*+ offsetX*/; //x
	transform->global_transformation[1][3] = transform->global_transformation[1][3] + offsetY; //y
	DrawTexture(isGame, barTexture);

	transform->global_transformation = matrix;
	DrawTexture(isGame, texture);
}

void ComponentBar::DrawTexture(bool isGame, ResourceTexture* tex)
{
	ComponentTransform* transform = (ComponentTransform*)game_object_attached->GetComponent(ComponentType::TRANSFORM);
	float4x4 matrix = transform->global_transformation;

	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);

	if (isGame && App->renderer3D->actual_game_camera != nullptr) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
#ifndef GAME_VERSION
		glOrtho(0, App->ui->panel_game->width, App->ui->panel_game->height, 0, App->renderer3D->actual_game_camera->frustum.farPlaneDistance, App->renderer3D->actual_game_camera->frustum.farPlaneDistance);
#else
		glOrtho(0, App->window->width, App->window->height, 0, App->renderer3D->actual_game_camera->frustum.farPlaneDistance, App->renderer3D->actual_game_camera->frustum.farPlaneDistance);
#endif
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		matrix[0][0] /= canvas->width * 0.5F;
		matrix[1][1] /= canvas->height * 0.5F;
		float3 canvas_pos = canvas_trans->GetGlobalPosition();
		float3 object_pos = transform->GetGlobalPosition();
		float3 canvasPivot = { canvas_pos.x - canvas->width * 0.5F, canvas_pos.y + canvas->height * 0.5F, 0 };
		float2 origin = float2((transform->global_transformation[0][3] - canvasPivot.x) / (canvas->width), (transform->global_transformation[1][3] - canvasPivot.y) / (canvas->height));

#ifndef GAME_VERSION
		x = origin.x * App->ui->panel_game->width;
		y = -origin.y * App->ui->panel_game->height;
#else
		x = origin.x * App->window->width;
		y = origin.y * App->window->height;
#endif

		origin.x = (origin.x - 0.5F) * 2;
		origin.y = -(-origin.y - 0.5F) * 2;
		matrix[0][3] = origin.x;
		matrix[1][3] = origin.y;
		matrix[2][3] = 0.0f;
	}

	if (tex != nullptr) {
		glAlphaFunc(GL_GREATER, 0.0f);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindTexture(GL_TEXTURE_2D, tex->id);
	}

	if(tex == texture)
		glColor4f(current_color.r, current_color.g, current_color.b, current_color.a);
	else
		glColor4f(bar_color.r, bar_color.g, bar_color.b, bar_color.a);

	if (transform->IsScaleNegative())
		glFrontFace(GL_CW);

	if (!canvas->isWorld)
	{
		glPushMatrix();
		glMultMatrixf(matrix.Transposed().ptr());
	}
	else
	{
		position.x = matrix[0][3];
		position.y = matrix[1][3];
		position.z = matrix[2][3];

		scale.x = matrix[0][0];
		scale.y = matrix[1][1];
		scale.z = matrix[2][2];


		float4x4 uiLocal = float4x4::FromTRS(position, game_object_attached->transform->GetGlobalRotation(), scale);
		float4x4 uiGlobal = uiLocal;

		/*	if (!particleInfo.globalTransform)
			{
				float4x4 parentGlobal = owner->emmitter.GetGlobalTransform();
				particleGlobal = parentGlobal * particleLocal;
			}*/

		glPushMatrix();
		glMultMatrixf((GLfloat*)&(uiGlobal.Transposed()));

	}

	glEnableClientState(GL_VERTEX_ARRAY);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindBuffer(GL_ARRAY_BUFFER, verticesID);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, uvID);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	if (transform->IsScaleNegative())
		glFrontFace(GL_CCW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
}

void ComponentBar::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("X", x);
	to_save->SetNumber("Y", y);
	to_save->SetNumber("Width", size.x);
	to_save->SetNumber("Height", size.y);

	to_save->SetBoolean("Enabled", enabled);
	to_save->SetNumber("Type", (int)type);
	to_save->SetNumber("UIType", (int)ui_type);
	to_save->SetString("TextureID", (texture != nullptr) ? std::to_string(texture->GetID()).data() : "0");
	to_save->SetString("sliderTexture", (barTexture != nullptr) ? std::to_string(barTexture->GetID()).data() : "0");
	to_save->SetColor("Color", current_color);
	to_save->SetColor("BarColor", bar_color);

	to_save->SetNumber("maxValue", maxValue);
	to_save->SetNumber("minValue", minValue);
	to_save->SetNumber("currentValue", currentValue);
	to_save->SetNumber("factor", factor);
	to_save->SetNumber("barScaleX", barScaleX);
	to_save->SetNumber("barScaleY", barScaleY);
	to_save->SetNumber("offsetX", offsetX);
	to_save->SetNumber("offsetY", offsetY);
	
}

void ComponentBar::LoadComponent(JSONArraypack* to_load)
{
	x = to_load->GetNumber("X");
	y = to_load->GetNumber("Y");
	size = { (float)to_load->GetNumber("Width"), (float)to_load->GetNumber("Height") };

	enabled = to_load->GetBoolean("Enabled");
	current_color = to_load->GetColor("Color");
	bar_color = to_load->GetColor("BarColor");

	maxValue = to_load->GetNumber("maxValue");
	minValue = to_load->GetNumber("minValue");
	currentValue = to_load->GetNumber("currentValue");
	factor = to_load->GetNumber("factor");
	barScaleX = to_load->GetNumber("barScaleX");
	barScaleY = to_load->GetNumber("barScaleY");
	offsetX = to_load->GetNumber("offsetX");
	offsetY = to_load->GetNumber("offsetY");


	u64 textureID = std::stoull(to_load->GetString("TextureID"));
	if (textureID != 0) {
		ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(textureID);
		if (tex != nullptr) {
			SetTexture(tex);
		}
	}
	u64 barTex = std::stoull(to_load->GetString("sliderTexture"));
	if (barTex != 0) {
		ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(barTex);
		if (tex != nullptr) {
			ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			if (tex != nullptr && tex != barTexture) {
				tex->IncreaseReferences();
				if (barTexture != nullptr) {
					barTexture->DecreaseReferences();
				}
				barTexture = tex;
			}
		}
	}
	GameObject* p = game_object_attached->parent;
	bool changed = true;
	while (changed) {
		if (p != nullptr) {
			ComponentCanvas* canvas = p->GetComponent<ComponentCanvas>();
			if (canvas != nullptr) {
				SetCanvas(canvas);
				changed = false;
			}
			p = p->parent;
		}
		else {
			changed = false;
			SetCanvas(nullptr);
		}
	}
}

void ComponentBar::CalculateFactor()
{
	if (currentValue > maxValue) currentValue = maxValue;
	if (currentValue < minValue) currentValue = minValue;


	factor = (((currentValue - minValue) * 100.0f) / (maxValue - minValue)) / 100.0f;
}

float ComponentBar::GetBarValue()
{
	return factor;
}

void ComponentBar::SetBarValue(float factor)
{
	if (factor < 0)
		factor = 0;
	else if (factor > 1)
		factor = 1;

	this->factor = factor;
	currentValue = (factor * (maxValue - minValue)) + minValue;
}

void ComponentBar::SetBarColor(float r, float g, float b, float a)
{
	bar_color = { r,g,b,a };
}

void ComponentBar::SetTextureBar(ResourceTexture* tex)
{
	if (tex != nullptr && tex != barTexture) {
		tex->IncreaseReferences();
		if (barTexture != nullptr) {
			barTexture->DecreaseReferences();
		}
		barTexture = tex;
		//SetSize(tex->width, tex->height);
	}
}

