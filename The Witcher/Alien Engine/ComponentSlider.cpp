#include "ComponentSlider.h"
#include "glew/include/glew.h"
#include "ComponentTransform.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include "ResourceTexture.h"
#include "GameObject.h"
#include "Application.h"
#include "PanelGame.h"
#include "ModuleUI.h"
#include "ModuleInput.h"
#include "ReturnZ.h"
#include "FileNode.h"
#include "PanelProject.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "mmgr/mmgr.h"

ComponentSlider::ComponentSlider(GameObject* obj) : ComponentUI(obj)
{
	ui_type = ComponentType::UI_SLIDER;
}

bool ComponentSlider::DrawInspector()
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

	if (ImGui::CollapsingHeader("Slider", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		RightClickMenu("Slider");

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

		/*----------SLIDER TEXTURE------------------*/
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text("Slider Texture");

		ImGui::SameLine(150);
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.16f, 0.29F, 0.5, 1 });
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.16f, 0.29F, 0.5, 1 });
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.16f, 0.29F, 0.5, 1 });
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);

		ImGui::Button((sliderTexture == nullptr) ? "NULL" : std::string(sliderTexture->GetName()).data(), { ImGui::GetWindowWidth() * 0.55F , 0 });

		if (ImGui::IsItemClicked() && sliderTexture != nullptr) {
			App->ui->panel_project->SelectFile(sliderTexture->GetAssetsPath(), App->resources->assets);
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
							if (tex != nullptr && tex != sliderTexture) {
								tex->IncreaseReferences();
								if (sliderTexture != nullptr) {
									sliderTexture->DecreaseReferences();
								}
								sliderTexture = tex;
							}
						}
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
		if (sliderTexture != nullptr) {
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
			if (ImGui::Button("X") && sliderTexture != nullptr) {
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
				if (sliderTexture != nullptr) {
					sliderTexture->DecreaseReferences();
					sliderTexture = nullptr;
				}
			}
			ImGui::PopStyleColor(3);
		}
		/*----------SLIDER TEXTURE------------------*/

		ImGui::Spacing();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text("Color");
		ImGui::SameLine(150);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		static bool set_Z = true;
		static Color col;
		col = current_color;
		if (ImGui::ColorEdit4("##RendererColor", &col, ImGuiColorEditFlags_Float)) {
			if (set_Z)
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			set_Z = false;
			current_color = col;
		}
		else if (!set_Z && ImGui::IsMouseReleased(0)) {
			set_Z = true;
		}
		ImGui::Spacing();
		ImGui::Text("Slider Scale");
		ImGui::SameLine(150);
		float sliderScale[] = { sliderScaleX, sliderScaleY };
		if (ImGui::DragFloat2("##Slider Scale", sliderScale, 0.1F)) {
			sliderScaleX = sliderScale[0];
			sliderScaleY = sliderScale[1];
		}
		ImGui::Spacing(); 	ImGui::Spacing(); 	ImGui::Spacing();
		ImGui::Text("Factor");
		ImGui::SameLine(150);
		float factor_slider = factor;
		if (ImGui::DragFloat("##Factor", &factor_slider, 0.01f, 0.0f, 1.0f, "%.3f")) {
			SetValue(factor_slider);
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}
	else {
		RightClickMenu("Slider");
	}

	return true;
}

void ComponentSlider::Draw(bool isGame)
{
	if (canvas == nullptr || canvas_trans == nullptr) {
		return;
	}
	ComponentTransform* transform = (ComponentTransform*)game_object_attached->GetComponent(ComponentType::TRANSFORM);
	float4x4 matrix = transform->global_transformation;
	transform->global_transformation[0][0] = transform->global_transformation[0][0] * sliderScaleX;
	transform->global_transformation[1][1] = transform->global_transformation[1][1] * sliderScaleY;
	transform->global_transformation[0][3] = (factor * ((matrix[0][3] + matrix[0][0]- transform->global_transformation[0][0]) - (matrix[0][3] - matrix[0][0]+ transform->global_transformation[0][0]))) + (matrix[0][3] - matrix[0][0]+ transform->global_transformation[0][0]);
	transform->global_transformation[1][3] = transform->global_transformation[1][3];

	
	sliderX = transform->global_transformation[0][3];


	DrawTexture(isGame, sliderTexture);
	transform->global_transformation = matrix;
	DrawTexture(isGame, texture);
}


void ComponentSlider::Update()
{
	if (Time::IsPlaying()) {
		UILogic();
		GetValue();

		switch (state)
		{
		case Idle:
			break;
		case Hover:
			OnHover();
			break;
		case Click:
			OnClick();
			break;
		case Pressed:
			OnPressed();
			break;
		case Release:
			OnRelease();
			break;
		default:
			break;
		}
	}
}


void ComponentSlider::DrawTexture(bool isGame, ResourceTexture* tex)
{
	ComponentTransform* transform = (ComponentTransform*)game_object_attached->GetComponent(ComponentType::TRANSFORM);
	float4x4 matrix = transform->global_transformation;

	glDisable(GL_CULL_FACE);

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
		if (tex == sliderTexture)
		{
			/*sliderX = origin.x * App->ui->panel_game->width;
			sliderY = -origin.y * App->ui->panel_game->height;*/
		}


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
	}

	if (tex != nullptr) {
		glAlphaFunc(GL_GREATER, 0.0f);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindTexture(GL_TEXTURE_2D, tex->id);
	}

	glColor4f(current_color.r, current_color.g, current_color.b, current_color.a);

	if (transform->IsScaleNegative())
		glFrontFace(GL_CW);

	glPushMatrix();
	glMultMatrixf(matrix.Transposed().ptr());

	glEnableClientState(GL_VERTEX_ARRAY);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindBuffer(GL_ARRAY_BUFFER, verticesID);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, uvID);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
	glDrawElements(GL_TRIANGLES, 6 * 3, GL_UNSIGNED_INT, 0);

	if (transform->IsScaleNegative())
		glFrontFace(GL_CCW);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();

	glEnable(GL_CULL_FACE);
}

void ComponentSlider::Reset()
{
}

void ComponentSlider::SetComponent(Component* component)
{
}

void ComponentSlider::Clone(Component* clone)
{
}

void ComponentSlider::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("X", x);
	to_save->SetNumber("Y", y);
	to_save->SetNumber("Width", size.x);
	to_save->SetNumber("Height", size.y);

	to_save->SetBoolean("Enabled", enabled);
	to_save->SetNumber("Type", (int)type);
	to_save->SetNumber("UIType", (int)ui_type);
	to_save->SetString("TextureID", (texture != nullptr) ? std::to_string(texture->GetID()) : "0");
	to_save->SetString("sliderTexture", (sliderTexture != nullptr) ? std::to_string(sliderTexture->GetID()) : "0");
	to_save->SetColor("Color", current_color);

	to_save->SetNumber("sliderScaleX", sliderScaleX);
	to_save->SetNumber("sliderScaleY", sliderScaleY);
	to_save->SetNumber("sliderX", sliderX);
	to_save->SetNumber("sliderY", sliderY);
	to_save->SetNumber("factor", factor);

	to_save->SetColor("ColorCurrent", current_color);
	to_save->SetColor("ColorIdle", idle_color);
	to_save->SetColor("ColorHover", hover_color);
	to_save->SetColor("ColorClicked", clicked_color);
	to_save->SetColor("ColorPressed", pressed_color);
	to_save->SetColor("ColorDisabled", disabled_color);

}

void ComponentSlider::LoadComponent(JSONArraypack* to_load)
{
	x = to_load->GetNumber("X");
	y = to_load->GetNumber("Y");
	size = { (float)to_load->GetNumber("Width"), (float)to_load->GetNumber("Height") };

	enabled = to_load->GetBoolean("Enabled");
	current_color = to_load->GetColor("Color");
	sliderScaleX = to_load->GetNumber("sliderScaleX");
	sliderScaleY = to_load->GetNumber("sliderScaleY");
	sliderX = to_load->GetNumber("sliderX");
	sliderY = to_load->GetNumber("sliderY");
	factor = to_load->GetNumber("factor");


	current_color = to_load->GetColor("ColorCurrent");
	idle_color = to_load->GetColor("ColorIdle");
	hover_color = to_load->GetColor("ColorHover");
	clicked_color = to_load->GetColor("ColorClicked");
	pressed_color = to_load->GetColor("ColorPressed");
	disabled_color = to_load->GetColor("ColorDisabled");

	u64 textureID = std::stoull(to_load->GetString("TextureID"));
	if (textureID != 0) {
		ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(textureID);
		if (tex != nullptr) {
			SetTexture(tex);
		}
	}
	u64 sliderTex = std::stoull(to_load->GetString("sliderTexture"));
	if (sliderTex != 0) {
			ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(sliderTex);
			if (tex != nullptr) {
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
				if (tex != nullptr && tex != sliderTexture) {
					tex->IncreaseReferences();
					if (sliderTexture != nullptr) {
						sliderTexture->DecreaseReferences();
					}
					sliderTexture = tex;
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
bool ComponentSlider::OnHover()
{
	current_color = hover_color;
	return true;
}

bool ComponentSlider::OnClick()
{
	current_color = clicked_color;
	return true;
}

bool ComponentSlider::OnPressed()
{
	ComponentTransform* trans = game_object_attached->GetComponent<ComponentTransform>();
	float width = (sliderX + ((trans->global_transformation[0][0] * sliderScaleX / (canvas->width * 0.5F)) * App->ui->panel_game->width) * 0.5F) - (sliderX - ((trans->global_transformation[0][0] * sliderScaleX / (canvas->width * 0.5F)) * App->ui->panel_game->width) * 0.5F);
	float width_bg = (x + ((trans->global_transformation[0][0] / (canvas->width * 0.5F)) * App->ui->panel_game->width) * 0.5F) - (x - ((trans->global_transformation[0][0] / (canvas->width * 0.5F)) * App->ui->panel_game->width) * 0.5F);
	
	int xmotion = App->input->GetMouseXMotion();
	
	if (xmotion > 0)
	{
		factor += (0.01f* xmotion);
	}
	if (xmotion < 0)
	{
		factor += (0.01f* xmotion);
	}
	if (factor>=1.0f)
	{
		factor = 1.0f;
	}
	if (factor <= 0.0f)
	{
		factor = 0.0f;
	}


	current_color = pressed_color;
	return true;
}

bool ComponentSlider::OnRelease()
{
	current_color = idle_color;
	return true;
}

void ComponentSlider::SetValue(float factor)
{
	if (factor >= 0.0f && factor <= 1.0f)
	{
		this->factor = factor;
	}
}

float ComponentSlider::GetValue()
{
	ComponentTransform* trans = game_object_attached->GetComponent<ComponentTransform>();
	float width_bg = (x + ((trans->global_transformation[0][0] / (canvas->width * 0.5F)) * App->ui->panel_game->width) * 0.5F) - (x - ((trans->global_transformation[0][0] / (canvas->width * 0.5F)) * App->ui->panel_game->width) * 0.5F);
	float width = (sliderX + ((trans->global_transformation[0][0] * sliderScaleX / (canvas->width * 0.5F)) * App->ui->panel_game->width) * 0.5F) - (sliderX - ((trans->global_transformation[0][0] * sliderScaleX / (canvas->width * 0.5F)) * App->ui->panel_game->width) * 0.5F);
	
	float startPos = (x - (width_bg * 0.5f));
	float endPos = (x + (width_bg * 0.5f)- width);
	float thumbPos = sliderX - width * 0.5f;
	LOG_ENGINE("SLIDER VALUE: %f", (thumbPos - startPos) / (endPos - startPos));
	return((thumbPos - startPos) / (endPos - startPos));

}




