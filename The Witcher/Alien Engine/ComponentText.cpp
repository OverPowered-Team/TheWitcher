#include "Application.h"
#include "ModuleResources.h"
#include "ModuleObjects.h"
#include "ModuleFileSystem.h"
#include "ModuleUI.h"
#include "PanelGame.h"
#include "ComponentTransform.h"
#include "ComponentText.h"
#include "ResourceFont.h"
#include "ResourceShader.h"
#include "Viewport.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "FileNode.h"
#include "ReturnZ.h"
#include "glew/include/glew.h"
#include "mmgr/mmgr.h"
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"

#include "MathGeoLib/include/Math/Matrix.inl"

ComponentText::ComponentText(GameObject* obj) : ComponentUI(obj)
{
	text = "Hola";
	if(!font)
		font = App->resources->default_font;

	GenerateVAOVBO();

	ui_type = ComponentType::UI_TEXT;
	tabbable = false;

	font->IncreaseReferences();
}

ComponentText::~ComponentText()
{
	font->DecreaseReferences();
}

bool ComponentText::DrawInspector()
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

	if (ImGui::CollapsingHeader("Text", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		RightClickMenu("Text");

		ImGui::Spacing();

		static char text_str[MAX_PATH];
		memcpy(text_str, text.data(), MAX_PATH);

		ImGui::Text("Text: "); ImGui::SameLine();
		if (ImGui::InputText("##Text", text_str, MAX_PATH, ImGuiInputTextFlags_AutoSelectAll)) {
			text = text_str;
		}

		static bool set_bg_Z = true;
		static Color bg_col;
		if (ImGui::ColorEdit4("##Color", &bg_col, ImGuiColorEditFlags_Float)) {
			if (set_bg_Z)
				ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			set_bg_Z = false;
			current_color = bg_col;
		}
		else if (!set_bg_Z && ImGui::IsMouseReleased(0)) {
			set_bg_Z = true;
		}

		ImGui::Spacing(); ImGui::Separator();

		ImGui::Text("Font");
		ImGui::SameLine();
		ImGui::Button(font ? font->name.data() : "No Font Assigned", { ImGui::GetWindowWidth() * 0.55F , 0 });
		if (ImGui::BeginDragDropTarget()) {
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_PROJECT_NODE, ImGuiDragDropFlags_SourceNoDisableHover);
			if (payload != nullptr && payload->IsDataType(DROP_ID_PROJECT_NODE)) {
				FileNode* node = *(FileNode**)payload->Data;
				if (node != nullptr && node->type == FileDropType::FONT) {
					std::string path = App->file_system->GetPathWithoutExtension(node->path + node->name);
					path += "_meta.alien";
					u64 ID = App->resources->GetIDFromAlienPath(path.data());
					if (ID != 0) {
						ResourceFont* fnt = (ResourceFont*)App->resources->GetResourceWithID(ID);
						if (fnt != nullptr) {
							font = fnt;
						}
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
		static int align_curr = (int)align;
		if (ImGui::Combo("Text Align", &align_curr, "Left\0Middle\0Right"))
		{
			align = (TextAlign)align_curr;
		}

		ImGui::Separator();

		ImGui::SetNextItemWidth(150); ImGui::DragInt("Width", &width, 5.f); ImGui::SameLine(); ImGui::SetNextItemWidth(150);
		ImGui::DragFloat("Interlineal", &interlineal, 0.25f);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}
	else {
		RightClickMenu("Text");
	}
	return true;
}

void ComponentText::Draw(bool isGame)
{
	if (font == nullptr || canvas_trans == nullptr || canvas == nullptr) {
		return;
	}

	ComponentTransform* transform = (ComponentTransform*)game_object_attached->GetComponent(ComponentType::TRANSFORM);
	float4x4 matrix = transform->global_transformation;
	float2 scale = float2(matrix[0][0], matrix[1][1]);

	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);

	if (isGame && App->renderer3D->actual_game_camera != nullptr) {
		font->text_ortho->Bind();
		font->text_ortho->SetUniformFloat3("textColor", float3(current_color.r, current_color.g, current_color.b));

		glm::mat4 projection;
		#ifndef GAME_VERSION
		projection = glm::ortho(0.0f, App->ui->panel_game->width, 0.0f, App->ui->panel_game->height);
		#else
		projection = glm::ortho(0.0f, (float)App->window->width, 0.0f, (float)App->window->height);
		#endif

		float4x4 proj_mat = float4x4(projection[0][0], projection[0][1], projection[0][2], projection[0][3],
			projection[1][0], projection[1][1], projection[1][2], projection[1][3],
			projection[2][0], projection[2][1], projection[2][2], projection[2][3],
			projection[3][0], projection[3][1], projection[3][2], projection[3][3]);

		float3 canvas_pos = canvas_trans->GetGlobalPosition();
		float3 object_pos = transform->GetGlobalPosition();
		float3 canvasPivot = {	canvas_pos.x - canvas->width * 0.5F, 
								canvas_pos.y + canvas->height * 0.5F,
								0 };

		float2 origin = float2((object_pos.x - canvasPivot.x) / (canvas->width), (canvasPivot.y + object_pos.y) / (canvas->height));

		#ifndef GAME_VERSION
		x = origin.x * App->ui->panel_game->width;
		y = origin.y * App->ui->panel_game->height;
		#else
		x = origin.x * App->window->width;
		y = origin.y * App->window->height;
		#endif

		font->text_ortho->SetUniformMat4f("projection", proj_mat);
	}
	else
	{
		font->text_shader->Bind();
		font->text_shader->SetUniformFloat3("textColor", float3(current_color.r, current_color.g, current_color.b));
		font->text_shader->SetUniformMat4f("projection", App->renderer3D->scene_fake_camera->GetProjectionMatrix4f4());
		font->text_shader->SetUniformMat4f("view", App->renderer3D->scene_fake_camera->GetViewMatrix4x4());
	}

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);
	
	std::string::const_iterator c;
	float pos_x = 0;
	float pos_y = 0;
	float line = 0;
	// Divides the current panel width by the canvas width in order to set a relation between the elements in the canvas and the ones in the panel
	// TODO2: Que cuando se este escribiendo no afecte al Gizmos
	// TODO4: Modo game la UI
	float factor_x = 0;
	float factor_y = 0;
	#ifndef GAME_VERSION
	factor_x = App->ui->panel_game->width / canvas->width;
	factor_y = App->ui->panel_game->height / canvas->height;
	#else
	factor_x = App->window->width / canvas->width;
	factor_y = App->window->height / canvas->height;
	#endif

	int current_size = 0;
	for (c = text.begin(); c != text.end(); c++) {
		Character ch = font->fontData.charactersMap[*c];
		static float xpos = 0;
		static float ypos = 0;
		static float w = 0;
		static float h = 0;

		switch (align)
		{
		case LEFT:
			if (isGame && App->renderer3D->actual_game_camera != nullptr)
			{
				xpos = x + pos_x + ch.bearing.x * scale.x * factor_x;
				ypos = y - pos_y - (ch.size.y - ch.bearing.y) * scale.y * factor_y;
				w = ch.size.x * scale.x * factor_x;
				h = ch.size.y * scale.y * factor_y;
			}
			else
			{
				xpos = matrix[0][3] + pos_x + ch.bearing.x * scale.x;
				ypos = matrix[1][3] - pos_y - (ch.size.y - ch.bearing.y) * scale.y;
				w = ch.size.x * scale.x;
				h = ch.size.y * scale.y;
			}
			break;
		case MIDDLE:
			if(c == text.begin())
				for (std::string::const_iterator aux_c = text.begin(); aux_c != text.end(); aux_c++)
					current_size += font->fontData.charactersMap[*aux_c].advance;

			if (isGame && App->renderer3D->actual_game_camera != nullptr)
			{
				xpos = x + pos_x + (ch.bearing.x + (width - current_size) * 0.5) * scale.x * factor_x;
				ypos = y - pos_y - (ch.size.y - ch.bearing.y) * scale.y * factor_y;
				w = ch.size.x * scale.x * factor_x;
				h = ch.size.y * scale.y * factor_y;
			}
			else
			{
				xpos = matrix[0][3] + pos_x + (ch.bearing.x + (width - current_size) * 0.5) * scale.x;
				ypos = matrix[1][3] - pos_y - (ch.size.y - ch.bearing.y) * scale.y;
				w = ch.size.x * scale.x;
				h = ch.size.y * scale.y;
			}
			break;
		case RIGHT:
			if (c == text.begin())
				for (std::string::const_iterator aux_c = text.begin(); aux_c != text.end(); aux_c++)
					current_size += font->fontData.charactersMap[*aux_c].advance;

			if (isGame && App->renderer3D->actual_game_camera != nullptr)
			{
				xpos = x + pos_x + (ch.bearing.x + (width - current_size)) * scale.x * factor_x;
				ypos = y - pos_y - (ch.size.y - ch.bearing.y) * scale.y * factor_y;
				w = ch.size.x * scale.x * factor_x;
				h = ch.size.y * scale.y * factor_y;
			}
			else
			{
				xpos = matrix[0][3] + pos_x + (ch.bearing.x + (width - current_size)) * scale.x;
				ypos = matrix[1][3] - pos_y - (ch.size.y - ch.bearing.y) * scale.y;
				w = ch.size.x * scale.x;
				h = ch.size.y * scale.y;
			}
			break;
		}
		

		float vertex[6][3] = {
			{ xpos,     ypos + h,	matrix[2][3]},
			{ xpos,     ypos,		matrix[2][3]},
			{ xpos + w, ypos,		matrix[2][3]},

			{ xpos,     ypos + h,   matrix[2][3]},
			{ xpos + w, ypos,       matrix[2][3]},
			{ xpos + w, ypos + h,   matrix[2][3]}
		};

		float uvs[6][2] = {
			{0.0, 0.0 },
			{0.0, 1.0 },
			{1.0, 1.0 },

			{0.0, 0.0 },
			{1.0, 1.0 },
			{1.0, 0.0 },
		};

		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.textureID);

		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, verticesID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex), vertex);
		glBindBuffer(GL_ARRAY_BUFFER, uvID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(uvs), uvs);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//Render loop line
		float3 pos = transform->GetGlobalPosition();

#ifndef GAME_VERSION
		if (!isGame)
		{
			glBegin(GL_LINE_LOOP);
			glVertex3f(pos.x, pos.y, pos.z);
			glVertex3f(pos.x + width * scale.x, pos.y, pos.z);
			glVertex3f(pos.x + width * scale.x, pos.y + font->fontData.charactersMap['l'].size.y * scale.y, pos.z);
			glVertex3f(pos.x, pos.y + font->fontData.charactersMap['l'].size.y * scale.y, pos.z);
			glEnd();
		}
#endif

		if (isGame && App->renderer3D->actual_game_camera != nullptr)
		{
			line = pos_x += ch.advance * scale.x * factor_x;
			if (line > width * scale.x * factor_x)
			{
				if (isGame && App->renderer3D->actual_game_camera != nullptr)
					pos_y += font->fontData.charactersMap['l'].size.y * scale.y * factor_y * interlineal;
				else
					pos_y += font->fontData.charactersMap['l'].size.y * scale.y * interlineal;

				pos_x = 0;
				current_size = 0;
			}
		}
		else
		{
			line = pos_x += ch.advance * scale.x;
			if (line > width * scale.x)
			{
				if (isGame && App->renderer3D->actual_game_camera != nullptr)
					pos_y += font->fontData.charactersMap['l'].size.y * scale.y * factor_y * interlineal;
				else
					pos_y += font->fontData.charactersMap['l'].size.y * scale.y * interlineal;

				pos_x = 0;
				current_size = 0;
			}
		}
	}
	
	font->text_shader->Unbind();
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
}

void ComponentText::Reset()
{
	current_color = Color::White();
	width = 200;
	interlineal = 1.5f;
	text = "Insert Text";
	font = App->resources->default_font;
}

void ComponentText::SetComponent(Component* component)
{
	component->enabled = enabled;
	ComponentUI* ui = (ComponentUI*)component;
	ui->current_color = current_color;

	dynamic_cast<ComponentText*>(component)->text = text;
	dynamic_cast<ComponentText*>(component)->width = width;
	dynamic_cast<ComponentText*>(component)->interlineal = interlineal;
	if (font != nullptr) {
		dynamic_cast<ComponentText*>(component)->font = font;
	}
	else {
		LOG_ENGINE("There's no default font!");
	}
}

void ComponentText::Clone(Component* clone)
{
	clone->enabled = enabled;
	ComponentUI* ui = (ComponentUI*)clone;
	ui->current_color = current_color;

	dynamic_cast<ComponentText*>(clone)->text = text;
	if (font != nullptr) {
		dynamic_cast<ComponentText*>(clone)->font = font;
	}
	else {
		LOG_ENGINE("There's no default font!");
	}

	GameObject* p = game_object_attached->parent;
	bool changed = true;
	while (changed) {
		if (p != nullptr) {
			ComponentCanvas* canvas = p->GetComponent<ComponentCanvas>();
			if (canvas != nullptr) {
				ui->SetCanvas(canvas);
				changed = false;
			}
			p = p->parent;
		}
		else {
			changed = false;
			ui->SetCanvas(nullptr);
		}
	}
}

void ComponentText::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetString("ID", std::to_string(ID).data());
	to_save->SetString("Text", text.data());
	to_save->SetNumber("UIType", (int)ui_type);
	if (font != nullptr) {
		to_save->SetString("FontID", std::to_string(font->GetID()).data());
	}
	to_save->SetBoolean("Enabled", enabled);
	to_save->SetColor("Color", current_color);
	to_save->SetNumber("Width", width);
	to_save->SetNumber("Interlineal", interlineal);
	to_save->SetNumber("Align", (int)align);
}

void ComponentText::LoadComponent(JSONArraypack* to_load)
{
	current_color = to_load->GetColor("Color");
	enabled = to_load->GetBoolean("Enabled");
	text = to_load->GetString("Text");
	interlineal = to_load->GetNumber("Interlineal");
	width = to_load->GetNumber("Width");
	int aux_type = (int)to_load->GetNumber("Align");
	align = (TextAlign)aux_type;

	u64 fontID = std::stoull(to_load->GetString("FontID"));
	if (fontID != 0) {
		font = (ResourceFont*)App->resources->GetResourceWithID(fontID);
		font->IncreaseReferences();
		LOG_ENGINE("Loaded font with ID %u", fontID);
	}
	else {
		LOG_ENGINE("Font ID equals to 0! Font not founded");
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

void ComponentText::SetFont(ResourceFont* font)
{
	this->font = font;
}

ResourceFont* ComponentText::GetFont() const
{
	return font;
}

void ComponentText::GenerateVAOVBO()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &verticesID);
	glGenBuffers(1, &uvID);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, verticesID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 3, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, uvID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 2, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}




