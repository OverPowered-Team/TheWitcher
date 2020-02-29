#include "Application.h"
#include "ModuleResources.h"
#include "ComponentTransform.h"
#include "ComponentText.h"
#include "ResourceFont.h"
#include "ReturnZ.h"
#include "glew/include/glew.h"
#include "mmgr/mmgr.h"

ComponentText::ComponentText(GameObject* obj) : ComponentUI(obj)
{
	text = "Hola";
	font = App->resources->default_font;

	uv[0] = { 0,0 };
	uv[1] = { 0,1 };
	uv[2] = { 1,1 };
	uv[3] = { 1,0 };

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uvID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * 4 * 2, uv, GL_STATIC_DRAW);

	ui_type = ComponentType::UI_TEXT;
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

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}
	else {
		RightClickMenu("Text");
	}
	return true;
}

bool ComponentText::DrawCharacter(Character ch)
{
	glBindTexture(GL_TEXTURE_2D, ch.textureID);

	glBindBuffer(GL_ARRAY_BUFFER, verticesID);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, uvID);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return true;
}

void ComponentText::Draw(bool isGame)
{
	glDisable(GL_CULL_FACE);

	glColor4f(0.5, 1.0, 1.0, 1.0);

	glPushMatrix();
	glMultMatrixf(game_object_attached->GetComponent<ComponentTransform>()->global_transformation.Transposed().ptr());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);

	glEnable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnableClientState(GL_VERTEX_ARRAY);

	std::string::const_iterator c;
	int i = 0;
	for(c = text.begin(); c != text.end(); c++) {
		Character ch = font->fontData.charactersMap[*c];
		float xpos = x + ch.bearing.x;
		float ypos = (ch.size.y - ch.bearing.y);
		float w = ch.size.x;
		float h = ch.size.y;

		vertices[0] = { xpos, ypos + h, 0 };
		vertices[1] = { xpos, ypos, 0 };
		vertices[2] = { xpos + w, ypos, 0 };
		vertices[3] = { xpos + w, ypos + h, 0 };
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, verticesID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * 4 * 3, vertices, GL_DYNAMIC_DRAW);

		DrawCharacter(ch);
		i++;
		x += ch.advance;
	}

	x = 0;

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();

	glDisable(GL_ALPHA_TEST);

	glEnable(GL_CULL_FACE);
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

	//TODO: UPDATE TEXT

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
	to_save->SetString("ID", std::to_string(ID));
	to_save->SetString("Text", text);
	to_save->SetNumber("UIType", (int)ui_type);
	if (font != nullptr) {
		to_save->SetString("FontID", std::to_string(font->GetID()));
	}
	to_save->SetBoolean("Enabled", enabled);
	to_save->SetColor("Color", current_color);
}

void ComponentText::LoadComponent(JSONArraypack* to_load)
{
	enabled = to_load->GetBoolean("Enabled");
	text = to_load->GetString("Text");

	u64 fontID = std::stoull(to_load->GetString("FontID"));
	if (fontID != 0) {
		ResourceFont* font = (ResourceFont*)App->resources->GetResourceWithID(fontID);
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




