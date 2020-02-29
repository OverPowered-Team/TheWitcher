#include "ComponentCheckbox.h"
#include "ComponentCanvas.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include "ComponentImage.h"
#include "ComponentTransform.h"
#include "GameObject.h"
#include "imgui/imgui.h"
#include "ComponentUI.h"
#include "ReturnZ.h"
#include "PanelGame.h"
#include "ResourceTexture.h"
#include "Application.h"
#include "PanelProject.h"
#include "ComponentTransform.h"
#include "mmgr/mmgr.h"

ComponentCheckbox::ComponentCheckbox(GameObject* obj) : ComponentUI(obj)
{
	ui_type = ComponentType::UI_CHECKBOX;

}

bool ComponentCheckbox::DrawInspector()
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

	if (ImGui::CollapsingHeader("Checkbox", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		RightClickMenu("Checkbox");

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

		/*----------CROSS TEXTURE------------------*/
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text("Cross Texture");

		ImGui::SameLine(150);
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.16f, 0.29F, 0.5, 1 });
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.16f, 0.29F, 0.5, 1 });
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.16f, 0.29F, 0.5, 1 });
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);

		ImGui::Button((crossTexture == nullptr) ? "NULL" : std::string(crossTexture->GetName()).data(), { ImGui::GetWindowWidth() * 0.55F , 0 });

		if (ImGui::IsItemClicked() && crossTexture != nullptr) {
			App->ui->panel_project->SelectFile(crossTexture->GetAssetsPath(), App->resources->assets);
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
							if (tex != nullptr && tex != crossTexture) {
								tex->IncreaseReferences();
								if (crossTexture != nullptr) {
									crossTexture->DecreaseReferences();
								}
								crossTexture = tex;
							}
						}
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
		if (crossTexture != nullptr) {
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
			if (ImGui::Button("X") && crossTexture != nullptr) {
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
				if (crossTexture != nullptr) {
					crossTexture->DecreaseReferences();
					crossTexture = nullptr;
				}
			}
			ImGui::PopStyleColor(3);
		}
		/*----------CROSS TEXTURE------------------*/
		/*----------TICK TEXTURE------------------*/
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text("Tick Texture");

		ImGui::SameLine(150);
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.16f, 0.29F, 0.5, 1 });
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.16f, 0.29F, 0.5, 1 });
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.16f, 0.29F, 0.5, 1 });
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);

		ImGui::Button((tickTexture == nullptr) ? "NULL" : std::string(tickTexture->GetName()).data(), { ImGui::GetWindowWidth() * 0.55F , 0 });

		if (ImGui::IsItemClicked() && tickTexture != nullptr) {
			App->ui->panel_project->SelectFile(tickTexture->GetAssetsPath(), App->resources->assets);
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
							if (tex != nullptr && tex != tickTexture) {
								tex->IncreaseReferences();
								if (tickTexture != nullptr) {
									tickTexture->DecreaseReferences();
								}
								tickTexture = tex;
							}
						}
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
		if (tickTexture != nullptr) {
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
			if (ImGui::Button("X") && tickTexture != nullptr) {
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
				if (tickTexture != nullptr) {
					tickTexture->DecreaseReferences();
					tickTexture = nullptr;
				}
			}
			ImGui::PopStyleColor(3);
		}
		/*----------TICK TEXTURE------------------*/

		ImGui::Spacing();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
		ImGui::Text("Color");
		ImGui::SameLine(85);
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
		float crossScale[] = { crossScaleX, crossScaleY };
		if (ImGui::DragFloat2("Cross Scale", crossScale, 0.1F)) {
			crossScaleX = crossScale[0];
			crossScaleY = crossScale[1];
		}
		float tickScale[] = { tickScaleX, tickScaleY };
		if (ImGui::DragFloat2("Tick Scale", tickScale, 0.1F)) {
			tickScaleX = tickScale[0];
			tickScaleY = tickScale[1];
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

void ComponentCheckbox::Draw(bool isGame)
{
	if (canvas == nullptr || canvas_trans == nullptr) {
		return;
	}
	ComponentTransform* transform = (ComponentTransform*)game_object_attached->GetComponent(ComponentType::TRANSFORM);
	float4x4 matrix = transform->global_transformation;
	if (clicked)
	{
		transform->global_transformation[0][0] = transform->global_transformation[0][0] * tickScaleX;
		transform->global_transformation[1][1] = transform->global_transformation[1][1] * tickScaleY;
	}
	else
	{
		transform->global_transformation[0][0] = transform->global_transformation[0][0] * crossScaleX;
		transform->global_transformation[1][1] = transform->global_transformation[1][1] * crossScaleY;
	}
	transform->global_transformation[0][3] = transform->global_transformation[0][3];
	transform->global_transformation[1][3] = transform->global_transformation[1][3];

	if (clicked)
		DrawTexture(isGame, tickTexture);
	else
		DrawTexture(isGame, crossTexture);

	transform->global_transformation = matrix;
	DrawTexture(isGame, texture);
}


void ComponentCheckbox::DrawTexture(bool isGame, ResourceTexture* tex)
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

bool ComponentCheckbox::OnHover()
{
	if (active)
	{
		current_color = hover_color;
	}
	return true;
}

bool ComponentCheckbox::OnClick()
{
	if (active)
	{ 
		clicked = !clicked;
		
		current_color = clicked_color;

		CallListeners(&listenersOnClick);
	}
	return true;
}

bool ComponentCheckbox::OnPressed()
{
	if (active)
	{
		current_color = pressed_color;
		CallListeners(&listenersOnClickRepeat);
	}
	return true;
}

bool ComponentCheckbox::OnRelease()
{
	if (active)
	{ 
		current_color = idle_color;
		CallListeners(&listenersOnRelease);
	}
	return true;
}

void ComponentCheckbox::SetActive(bool active)
{
	this->active = active;
	if (active) {
		current_color = idle_color;
	}
	else {
		current_color = disabled_color;
	}
}

void ComponentCheckbox::AddListenerOnHover(std::function<void()> funct)
{
	listenersOnHover.push_back(funct);
}

void ComponentCheckbox::AddListenerOnClick(std::function<void()> funct)
{
	listenersOnClick.push_back(funct);
}

void ComponentCheckbox::AddListenerOnClickRepeat(std::function<void()> funct)
{
	listenersOnClickRepeat.push_back(funct);
}

void ComponentCheckbox::AddListenerOnRelease(std::function<void()> funct)
{
	listenersOnRelease.push_back(funct);
}




void ComponentCheckbox::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("X", x);
	to_save->SetNumber("Y", y);
	to_save->SetNumber("Width", size.x);
	to_save->SetNumber("Height", size.y);

	to_save->SetBoolean("Enabled", enabled);
	to_save->SetBoolean("clicked", clicked);
	to_save->SetNumber("Type", (int)type);
	to_save->SetNumber("UIType", (int)ui_type);
	to_save->SetString("TextureID", (texture != nullptr) ? std::to_string(texture->GetID()) : "0");
	to_save->SetString("crossTexture", (crossTexture != nullptr) ? std::to_string(crossTexture->GetID()) : "0");
	to_save->SetString("tickTexture", (tickTexture != nullptr) ? std::to_string(tickTexture->GetID()) : "0");
	to_save->SetColor("Color", current_color);

	to_save->SetNumber("crossScaleX", crossScaleX);
	to_save->SetNumber("crossScaleY", crossScaleY);
	to_save->SetNumber("tickScaleX", tickScaleX);
	to_save->SetNumber("tickScaleY", tickScaleY);
	

	to_save->SetColor("ColorCurrent", current_color);
	to_save->SetColor("ColorIdle", idle_color);
	to_save->SetColor("ColorHover", hover_color);
	to_save->SetColor("ColorClicked", clicked_color);
	to_save->SetColor("ColorPressed", pressed_color);
	to_save->SetColor("ColorDisabled", disabled_color);
}

void ComponentCheckbox::LoadComponent(JSONArraypack* to_load)
{
	x = to_load->GetNumber("X");
	y = to_load->GetNumber("Y");
	size = { (float)to_load->GetNumber("Width"), (float)to_load->GetNumber("Height") };

	enabled = to_load->GetBoolean("Enabled");
	clicked = to_load->GetBoolean("clicked");
	current_color = to_load->GetColor("Color");
	crossScaleX = to_load->GetNumber("crossScaleX");
	crossScaleY = to_load->GetNumber("crossScaleY");
	tickScaleX = to_load->GetNumber("tickScaleX");
	tickScaleY = to_load->GetNumber("tickScaleY");
	

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
	u64 crossTex = std::stoull(to_load->GetString("crossTexture"));
	if (crossTex != 0) {
		ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(crossTex);
		if (tex != nullptr) {
			ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			if (tex != nullptr && tex != crossTexture) {
				tex->IncreaseReferences();
				if (crossTexture != nullptr) {
					crossTexture->DecreaseReferences();
				}
				crossTexture = tex;
			}
		}
	}
	u64 tickTex = std::stoull(to_load->GetString("tickTexture"));
	if (tickTex != 0) {
		ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(tickTex);
		if (tex != nullptr) {
			ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			if (tex != nullptr && tex != tickTexture) {
				tex->IncreaseReferences();
				if (tickTexture != nullptr) {
					tickTexture->DecreaseReferences();
				}
				tickTexture = tex;
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

void ComponentCheckbox::CallListeners(std::vector<std::function<void()>>* listeners)
{
	if (listeners != nullptr) {
		auto item = listeners->begin();
		for (; item != listeners->end(); ++item) {
			if (*item != nullptr) {
				try {
					(*item)();
				}
				catch (...) {
					#ifndef GAME_VERSION
					LOG_ENGINE("Error when calling a listener function of a button");
					App->ui->SetError();
					#endif
				}
			}
		}
	}
}

ComponentCanvas* ComponentCheckbox::GetCanvas()
{
	ComponentCanvas* canvas = App->objects->GetRoot(true)->GetCanvas();
	if (canvas == nullptr) {
		GameObject* obj = new GameObject(App->objects->GetRoot(false));
		obj->SetName("Canvas");
		obj->AddComponent(new ComponentTransform(obj, { 0,0,0 }, { 0,0,0,0 }, { 1,1,1 }));
		canvas = new ComponentCanvas(obj);
		obj->AddComponent(canvas);
	}
	return canvas;
}
