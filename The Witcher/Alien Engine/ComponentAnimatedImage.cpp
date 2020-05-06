#include "Application.h"
#include "ModuleUI.h"
#include "PanelProject.h"
#include "ComponentTransform.h"
#include "ModuleResources.h"
#include "ModuleRenderer3D.h"
#include "ComponentAnimatedImage.h"
#include "ResourceTexture.h"
#include "ModuleWindow.h"
#include "PanelGame.h"
#include "mmgr/mmgr.h"


ComponentAnimatedImage::ComponentAnimatedImage(GameObject* obj): ComponentUI(obj)
{
	ui_type = ComponentType::UI_ANIMATED_IMAGE;
	tabbable = false;
}

bool ComponentAnimatedImage::DrawInspector()
{
	static bool check;

	ImGui::PushID(this);
	check = enabled;
	if (ImGui::Checkbox("##CmpActive", &check)) {
		//ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
		enabled = check;
	}
	ImGui::PopID();
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Animated Image", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		RightClickMenu("Animated Image");

		ImGui::Spacing();
		ImGui::Text("Add Image");
		ImGui::SameLine(85);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.0F,0.65F,0.0F,1.0F });
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.0F,0.8F,0.0F,1.0F });
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.0F,0.95F,0.0F,1.0F });
		if (ImGui::Button("+"))
		{
			//images.reserve(images.size() + 1);
			images.push_back(nullptr);
			last_frame++;
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine(105);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0.0F,0.0F,1.0F });
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0.0F,0.0F,1.0F });
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0.0F,0.0F,1.0F });
		if (ImGui::Button("-"))
		{
			if (!images.empty())
			{
				auto item = images.begin() + last_frame - 1;
				ClearTextureArray((*item));
				images.erase(item);
				last_frame--;
			}
		}
		ImGui::PopStyleColor(3);

		ImGui::Spacing();
		for (auto item = images.begin(); item != images.end(); ++item)
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Texture");

			ImGui::SameLine(85);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);

			ImGui::Button(((*item) == nullptr) ? "NULL" : std::string((*item)->GetName()).data(), { ImGui::GetWindowWidth() * 0.55F , 0 });

			if (ImGui::IsItemClicked() && (*item) != nullptr) {
				App->ui->panel_project->SelectFile((*item)->GetAssetsPath(), App->resources->assets);
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
								//ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
								(*item) = SetTextureArray(texture, (*item));
							}
						}
					}
				}
				ImGui::EndDragDropTarget();
			}
			if ((*item) != nullptr) {
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
				ImGui::PushID(std::distance(images.begin(), item) + 964723);
				if (ImGui::Button("X") && (*item) != nullptr) {
					//ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
					(*item) = ClearTextureArray((*item));
				}
				ImGui::PopID();
				ImGui::PopStyleColor(3);
			}
		}
		ImGui::Spacing();
		ImGui::Text("Loop");
		ImGui::SameLine(85);
		ImGui::Checkbox("##Loop", &loop);
		ImGui::Spacing();
		ImGui::Text("Speed");
		ImGui::SameLine(85);
		ImGui::DragFloat("##Speed", &speed, 0.5F, 0.1f, 100.0f, "%.1f");
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
				//ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			set_Z = false;
			current_color = col;
		}
		else if (!set_Z && ImGui::IsMouseReleased(0)) {
			set_Z = true;
		}
		ImGui::Spacing();


		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}
	else {
		RightClickMenu("Animated Image");
	}

	return true;
}

void ComponentAnimatedImage::Draw(bool isGame)
{
	if (canvas == nullptr || canvas_trans == nullptr) {
		return;
	}

	ComponentTransform* transform = (ComponentTransform*)game_object_attached->GetComponent(ComponentType::TRANSFORM);
	float4x4 matrix = transform->global_transformation;

	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);

	if (isGame && App->renderer3D->actual_game_camera != nullptr && !canvas->isWorld) {
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
		float2 origin = float2((object_pos.x - canvasPivot.x) / (canvas->width), (object_pos.y - canvasPivot.y) / (canvas->height));

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

	if (!images.empty()) {
		//glAlphaFunc(GL_GREATER, 0.0f);
		ResourceTexture* tex = GetCurrentFrame(Time::GetDT());
		if (tex != nullptr)
		{
			SetSize(tex->width, tex->height);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glBindTexture(GL_TEXTURE_2D, tex->id);
		}
	}

	glColor4f(current_color.r, current_color.g, current_color.b, current_color.a);

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

void ComponentAnimatedImage::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Width", size.x);
	to_save->SetNumber("Height", size.y);
	to_save->SetNumber("Speed", speed);
	to_save->SetBoolean("Loop", loop);
	to_save->SetBoolean("Enabled", enabled);
	to_save->SetNumber("Type", (int)type);
	to_save->SetNumber("UIType", (int)ui_type);
	to_save->SetColor("ColorCurrent", current_color);

	to_save->SetBoolean("HasAnimatedImages", !images.empty());
	if (!images.empty()) {
		JSONArraypack* imagesArray = to_save->InitNewArray("AnimatedImages");
		auto item = images.begin();
		for (; item != images.end(); ++item) {
			imagesArray->SetAnotherNode();
			imagesArray->SetString(std::to_string(item - images.begin()).data(), ((*item) != nullptr) ? std::to_string((*item)->GetID()).data() : "0");
		}
	}
	
}

void ComponentAnimatedImage::LoadComponent(JSONArraypack* to_load)
{
	size = { (float)to_load->GetNumber("Width"), (float)to_load->GetNumber("Height") };
	enabled = to_load->GetBoolean("Enabled");
	loop = to_load->GetBoolean("Loop");
	speed = (float)to_load->GetNumber("Speed");
	current_color = to_load->GetColor("ColorCurrent");
	current_frame = 0.0f;
	last_frame = 0;

	if (to_load->GetBoolean("HasAnimatedImages")) {
		JSONArraypack* imagesVector = to_load->GetArray("AnimatedImages");
		for (int i = 0; i < imagesVector->GetArraySize(); ++i) {
			u64 textureID = std::stoull(imagesVector->GetString(std::to_string(i).data()));
			if (textureID != 0) {
				ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(textureID);
				if (tex != nullptr) {
					images.push_back(nullptr);
					images.at(i) = SetTextureArray(tex, images.at(i));
					last_frame++;
				}
			}
			else
			{
				images.push_back(nullptr);
				last_frame++;
			}
			imagesVector->GetAnotherNode();
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

ResourceTexture* ComponentAnimatedImage::ClearTextureArray(ResourceTexture* item)
{
	if (item != nullptr) {
		item->DecreaseReferences();
		item = nullptr;
		return nullptr;
	}
	return nullptr;
}

ResourceTexture* ComponentAnimatedImage::SetTextureArray(ResourceTexture* tex, ResourceTexture* item)
{
	if (tex != nullptr && tex != item) {
		tex->IncreaseReferences();
		if (item != nullptr) {
			item->DecreaseReferences();
		}
		return tex;
	}
	return nullptr;
}

void ComponentAnimatedImage::SetAnimSpeed(float speed)
{
	this->speed = speed;
}

float ComponentAnimatedImage::GetAnimSpeed()
{
	return speed;
}

ResourceTexture* ComponentAnimatedImage::GetCurrentFrame(float dt)
{
	current_frame += speed * dt;
	if (current_frame >= last_frame)
	{
		current_frame = (loop) ? 0.0f : last_frame - 1;
		loops++;
	}
	return images.at((int)current_frame);
}

bool ComponentAnimatedImage::Finished() const
{
	return loops > 0;
}

void ComponentAnimatedImage::Reset()
{
	loops = 0;
	current_frame = 0.0f;
}

int ComponentAnimatedImage::SeeCurrentFrame()
{
	return (int)current_frame;
}
