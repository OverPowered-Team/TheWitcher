#include "PanelPhysics.h"
#include "ShortCutManager.h"
#include "ModulePhysX.h"
#include "Optick/include/optick.h"

PanelPhysics::PanelPhysics(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat, const SDL_Scancode& key3_repeat_extra) :
	Panel(panel_name, key1_down, key2_repeat, key3_repeat_extra)
{
	shortcut = App->shortcut_manager->AddShortCut("Panel Physics", key1_down, std::bind(&Panel::ChangeEnable, this), key2_repeat, key3_repeat_extra);
	enabled = false;
}

PanelPhysics::~PanelPhysics()
{
}

void PanelPhysics::PanelLogic()
{
	OPTICK_EVENT();

	ImGui::PushID(this);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(450, 900));

	ModulePhysX* physics = App->physx;
	CollisionLayers& collision_layers = App->physx->layers;
	std::vector<std::string> names = collision_layers.names;

	if (ImGui::Begin(panel_name.data(), &enabled))
	{
		float3 current_gravity = physics->gravity;

		ImGui::Spacing();
		ImGui::Title("Gravity");	if (ImGui::DragFloat3("##gravity", current_gravity.ptr(), 0.05f)) { physics->SetGravity(current_gravity); }
		ImGui::Title("Draw All");	ImGui::Checkbox("##debug_all_physics", &App->physx->debug_physics);

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Title("Layer");


		if (ImGui::BeginComboEx(std::string("##layers").c_str(), std::string(" " + names[current_layer]).c_str(), 200, ImGuiComboFlags_NoArrowButton))
		{
			for (int n = 0; n < names.size(); ++n)
			{
				bool is_selected = (current_layer == n);

				if (ImGui::Selectable(std::string(" " + names[n]).c_str(), is_selected))
					current_layer = n;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		ImGui::Title("");

		if (ImGui::Button("+")) open_add_popup = true;

		if (current_layer != 0)
		{
			ImGui::SameLine();
			if (ImGui::Button("-"))	 open_remove_popup = true;
		}


		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::NewLine();

		int size = names.size();
		int separation = 150;

		for (int i = size - 1; i >= 0; --i)
		{
			ImGui::SameLine(separation);
			ImGui::Text(std::string(std::to_string(i) + ".").c_str());
			separation += 24;
		}

		for (int i = 0; i < size; i++)
		{
			ImGui::Title(std::string(std::to_string(i) + ". " + names[i]).c_str());
			separation = 150;

			for (int j = size - 1; j >= i; j--)
			{
				bool value = collision_layers.data[i][j];
				ImGui::SameLine(separation);
				ImGui::PushID(&collision_layers.data[i][j]);

				if (ImGui::Checkbox(("##" + std::to_string(i) + std::to_string(j)).c_str(), &value))
				{
					collision_layers.SetLayers(i, j, collision_layers.data[i][j]);
					App->SendAlienEvent(&LayerChangedData(i, j), AlienEventType::COLLISION_LAYER_STATE_CHANGED);
				}
					
				ImGui::PopID();
				separation += 24;
			}
		}

	}

	ImGui::End();
	ImGui::PopStyleVar();

	PopUpAddLayer();
	PopUpRemoveLayer();

	ImGui::PopID();

}

void PanelPhysics::PopUpAddLayer()
{
	static char layer_name[40] = "";

	if (open_add_popup)
	{
		ImGui::OpenPopup("  Add Layer");
		ImGui::SetNextWindowSize({ 410,90 });

		if (ImGui::BeginPopupModal("  Add Layer", &open_add_popup, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::Title("Name");
			ImGui::InputText("##name_layer", layer_name, 40);

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::SetCursorPosX(160);
			if (ImGui::Button("Acept")) {
				open_add_popup = false;
				App->physx->layers.AddLayer(std::string(layer_name));
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				open_add_popup = false;
			}

			ImGui::EndPopup();
		}
	}
}

void PanelPhysics::PopUpRemoveLayer()
{
	if (open_remove_popup)
	{
		ImGui::OpenPopup("  Add Layer");
		ImGui::SetNextWindowSize({ 410,90 });

		if (ImGui::BeginPopupModal("  Add Layer", &open_remove_popup, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::Text(("  Do you want to delete \"" + App->physx->layers.names[current_layer] + "\" ?").data());

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::SetCursorPosX(160);
			if (ImGui::Button("Acept")) {
				App->physx->layers.RemoveLayer(App->physx->layers.names[current_layer]);
				App->SendAlienEvent( &App->physx->layers.names[current_layer], AlienEventType::COLLISION_LAYER_STATE_CHANGED);
				current_layer = 0;
				open_remove_popup = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				open_remove_popup = false;
			}

			ImGui::EndPopup();
		}
	}
}