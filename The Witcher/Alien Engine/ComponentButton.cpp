#include "ComponentButton.h"
#include "ComponentImage.h"
#include "imgui/imgui.h"
#include "ComponentUI.h"
#include "ComponentCanvas.h"
#include "ComponentAudioEmitter.h"
#include "ComponentTransform.h"
#include "ModuleRenderer3D.h"
#include "ModuleUI.h"
#include "ReturnZ.h"
#include "ComponentScript.h"
#include "ResourceTexture.h"
#include "Application.h"
#include "PanelProject.h"
#include "PanelGame.h"
#include "ModuleResources.h"
#include "ModuleUI.h"
#include "ModuleWindow.h"
#include "mmgr/mmgr.h"

ComponentButton::ComponentButton(GameObject* obj) :ComponentUI(obj)
{
	ui_type = ComponentType::UI_BUTTON;
	tabbable = true;
	current_tex_array = &idle_info;
}

void ComponentButton::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Width", size.x);
	to_save->SetNumber("Height", size.y);

	to_save->SetBoolean("Enabled", enabled);

	to_save->SetNumber("UIType", (int)ui_type);

	to_save->SetColor("ColorCurrent", current_color);
	to_save->SetColor("ColorIdle", idle_color);
	to_save->SetColor("ColorHover", hover_color);
	to_save->SetColor("ColorClicked", clicked_color);
	to_save->SetColor("ColorPressed", pressed_color);
	to_save->SetColor("ColorDisabled", disabled_color);

	to_save->SetString("ClickEvent", click_event.data());
	to_save->SetString("MoveEvent", move_event.data());

	to_save->SetBoolean("LoopIdle", idle_info.loop);
	to_save->SetNumber("AnimSpeedIdle", idle_info.speed);
	to_save->SetBoolean("LoopHover", hover_info.loop);
	to_save->SetNumber("AnimSpeedHover", hover_info.speed);
	to_save->SetBoolean("LoopClicked", clicked_info.loop);
	to_save->SetNumber("AnimSpeedClicked", clicked_info.speed);
	to_save->SetBoolean("LoopPressed", pressed_info.loop);
	to_save->SetNumber("AnimSpeedPressed", pressed_info.speed);
	to_save->SetBoolean("LoopDisabled", disabled_info.loop);
	to_save->SetNumber("AnimSpeedDisabled", disabled_info.speed);
	//---------------------------------------------------------

	to_save->SetBoolean("HasAnimatedIdleImages", !idle_info.tex_array.empty());
	if (!idle_info.tex_array.empty()) {
		JSONArraypack* imagesArray = to_save->InitNewArray("AnimatedIdleImages");
		auto item = idle_info.tex_array.begin();
		for (; item != idle_info.tex_array.end(); ++item) {
			imagesArray->SetAnotherNode();
			imagesArray->SetString(std::to_string(item - idle_info.tex_array.begin()).data(), ((*item) != nullptr) ? std::to_string((*item)->GetID()).data() : "0");
		}
	}

	to_save->SetBoolean("HasAnimatedHoverImages", !hover_info.tex_array.empty());
	if (!hover_info.tex_array.empty()) {
		JSONArraypack* imagesArray = to_save->InitNewArray("AnimatedHoverImages");
		auto item = hover_info.tex_array.begin();
		for (; item != hover_info.tex_array.end(); ++item) {
			imagesArray->SetAnotherNode();
			imagesArray->SetString(std::to_string(item - hover_info.tex_array.begin()).data(), ((*item) != nullptr) ? std::to_string((*item)->GetID()).data() : "0");
		}
	}

	to_save->SetBoolean("HasAnimatedClickedImages", !clicked_info.tex_array.empty());
	if (!clicked_info.tex_array.empty()) {
		JSONArraypack* imagesArray = to_save->InitNewArray("AnimatedClickedImages");
		auto item = clicked_info.tex_array.begin();
		for (; item != clicked_info.tex_array.end(); ++item) {
			imagesArray->SetAnotherNode();
			imagesArray->SetString(std::to_string(item - clicked_info.tex_array.begin()).data(), ((*item) != nullptr) ? std::to_string((*item)->GetID()).data() : "0");
		}
	}

	to_save->SetBoolean("HasAnimatedPressedImages", !pressed_info.tex_array.empty());
	if (!pressed_info.tex_array.empty()) {
		JSONArraypack* imagesArray = to_save->InitNewArray("AnimatedPressedImages");
		auto item = pressed_info.tex_array.begin();
		for (; item != pressed_info.tex_array.end(); ++item) {
			imagesArray->SetAnotherNode();
			imagesArray->SetString(std::to_string(item - pressed_info.tex_array.begin()).data(), ((*item) != nullptr) ? std::to_string((*item)->GetID()).data() : "0");
		}
	}

	to_save->SetBoolean("HasAnimatedDisabledImages", !disabled_info.tex_array.empty());
	if (!disabled_info.tex_array.empty()) {
		JSONArraypack* imagesArray = to_save->InitNewArray("AnimatedDisabledImages");
		auto item = disabled_info.tex_array.begin();
		for (; item != disabled_info.tex_array.end(); ++item) {
			imagesArray->SetAnotherNode();
			imagesArray->SetString(std::to_string(item - disabled_info.tex_array.begin()).data(), ((*item) != nullptr) ? std::to_string((*item)->GetID()).data() : "0");
		}
	}

	//---------------------------------------------------------
	//---------------------------------------------------------
	//---------------------------------------------------------
	to_save->SetBoolean("HasListenersOnClick", !listenersOnClick.empty());
	if (!listenersOnClick.empty()) {
		JSONArraypack* onClickArray = to_save->InitNewArray("ListenersOnClick");
		auto item = listenersOnClick.begin();
		for (; item != listenersOnClick.end(); ++item) {
			onClickArray->SetAnotherNode();
			onClickArray->SetString(std::to_string(item - listenersOnClick.begin()).data(), (*item).first.data());
		}
	}

	to_save->SetBoolean("HasListenersOnHover", !listenersOnHover.empty());
	if (!listenersOnHover.empty()) {
		JSONArraypack* onHoverArray = to_save->InitNewArray("ListenersOnHover");
		auto item = listenersOnHover.begin();
		for (; item != listenersOnHover.end(); ++item) {
			onHoverArray->SetAnotherNode();
			onHoverArray->SetString(std::to_string(item - listenersOnHover.begin()).data(), (*item).first.data());
		}
	}

	to_save->SetBoolean("HasListenersOnClickRepeat", !listenersOnClickRepeat.empty());
	if (!listenersOnClickRepeat.empty()) {
		JSONArraypack* onClickRepeatArray = to_save->InitNewArray("ListenersOnClickRepeat");
		auto item = listenersOnClickRepeat.begin();
		for (; item != listenersOnClickRepeat.end(); ++item) {
			onClickRepeatArray->SetAnotherNode();
			onClickRepeatArray->SetString(std::to_string(item - listenersOnClickRepeat.begin()).data(), (*item).first.data());
		}
	}

	to_save->SetBoolean("HasListenersOnRelease", !listenersOnRelease.empty());
	if (!listenersOnRelease.empty()) {
		JSONArraypack* onReleaseArray = to_save->InitNewArray("ListenersOnRelease");
		auto item = listenersOnRelease.begin();
		for (; item != listenersOnRelease.end(); ++item) {
			onReleaseArray->SetAnotherNode();
			onReleaseArray->SetString(std::to_string(item - listenersOnRelease.begin()).data(), (*item).first.data());
		}
	}

	to_save->SetBoolean("HasListenersOnExit", !listenersOnExit.empty());
	if (!listenersOnExit.empty()) {
		JSONArraypack* onExitArray = to_save->InitNewArray("ListenersOnExit");
		auto item = listenersOnExit.begin();
		for (; item != listenersOnExit.end(); ++item) {
			onExitArray->SetAnotherNode();
			onExitArray->SetString(std::to_string(item - listenersOnExit.begin()).data(), (*item).first.data());
		}
	}

	to_save->SetBoolean("HasListenersOnEnter", !listenersOnEnter.empty());
	if (!listenersOnEnter.empty()) {
		JSONArraypack* onEnterArray = to_save->InitNewArray("ListenersOnEnter");
		auto item = listenersOnEnter.begin();
		for (; item != listenersOnEnter.end(); ++item) {
			onEnterArray->SetAnotherNode();
			onEnterArray->SetString(std::to_string(item - listenersOnEnter.begin()).data(), (*item).first.data());
		}
	}
	//---------------------------------------------------------------
	to_save->SetString("SelectOnUp", std::to_string(select_on_up).data());
	to_save->SetString("SelectOnDown", std::to_string(select_on_down).data());
	to_save->SetString("SelectOnRight", std::to_string(select_on_right).data());
	to_save->SetString("SelectOnLeft", std::to_string(select_on_left).data());
}

void ComponentButton::LoadComponent(JSONArraypack* to_load)
{
	size = { (float)to_load->GetNumber("Width"), (float)to_load->GetNumber("Height") };

	enabled = to_load->GetBoolean("Enabled");

	current_color = to_load->GetColor("ColorCurrent");
	idle_color = to_load->GetColor("ColorIdle");
	hover_color = to_load->GetColor("ColorHover");
	clicked_color = to_load->GetColor("ColorClicked");
	pressed_color = to_load->GetColor("ColorPressed");
	disabled_color = to_load->GetColor("ColorDisabled");

	click_event = to_load->GetString("ClickEvent", "none");
	move_event = to_load->GetString("MoveEvent", "none");

	idle_info.loop = to_load->GetBoolean("LoopIdle");
	idle_info.speed = to_load->GetNumber("AnimSpeedIdle");
	hover_info.loop = to_load->GetBoolean("LoopHover");
	hover_info.speed = to_load->GetNumber("AnimSpeedHover");
	clicked_info.loop = to_load->GetBoolean("LoopClicked");
	clicked_info.speed = to_load->GetNumber("AnimSpeedClicked");
	pressed_info.loop = to_load->GetBoolean("LoopPressed");
	pressed_info.speed = to_load->GetNumber("AnimSpeedPressed");
	disabled_info.loop = to_load->GetBoolean("LoopDisabled");
	disabled_info.speed = to_load->GetNumber("AnimSpeedDisabled");

	idle_info.loop = to_load->GetBoolean("LoopIdle");
	idle_info.speed = to_load->GetNumber("AnimSpeedIdle");
	hover_info.loop = to_load->GetBoolean("LoopHover");
	hover_info.speed = to_load->GetNumber("AnimSpeedHover");
	clicked_info.loop = to_load->GetBoolean("LoopClicked");
	clicked_info.speed = to_load->GetNumber("AnimSpeedClicked");
	pressed_info.loop = to_load->GetBoolean("LoopPressed");
	pressed_info.speed = to_load->GetNumber("AnimSpeedPressed");
	disabled_info.loop = to_load->GetBoolean("LoopDisabled");
	disabled_info.speed = to_load->GetNumber("AnimSpeedDisabled");

	//-----------------------------------------------------------

	select_on_up = std::stoull(to_load->GetString("SelectOnUp"));
	select_on_down = std::stoull(to_load->GetString("SelectOnDown"));
	select_on_right = std::stoull(to_load->GetString("SelectOnRight"));
	select_on_left = std::stoull(to_load->GetString("SelectOnLeft"));

	//-------------------------------------------------------------

	if (to_load->GetBoolean("HasAnimatedIdleImages")) {
		JSONArraypack* imagesVector = to_load->GetArray("AnimatedIdleImages");
		for (int i = 0; i < imagesVector->GetArraySize(); ++i) {
			u64 textureID = std::stoull(imagesVector->GetString(std::to_string(i).data()));
			if (textureID != 0) {
				ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(textureID);
				if (tex != nullptr) {
					idle_info.tex_array.push_back(nullptr);
					idle_info.tex_array.at(i) = SetTextureArray(tex, idle_info.tex_array.at(i));
					idle_info.last_frame++;
				}
			}
			else
			{
				idle_info.tex_array.push_back(nullptr);
				idle_info.last_frame++;
			}
			imagesVector->GetAnotherNode();
		}
	}

	if (to_load->GetBoolean("HasAnimatedHoverImages")) {
		JSONArraypack* imagesVector = to_load->GetArray("AnimatedHoverImages");
		for (int i = 0; i < imagesVector->GetArraySize(); ++i) {
			u64 textureID = std::stoull(imagesVector->GetString(std::to_string(i).data()));
			if (textureID != 0) {
				ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(textureID);
				if (tex != nullptr) {
					hover_info.tex_array.push_back(nullptr);
					hover_info.tex_array.at(i) = SetTextureArray(tex, hover_info.tex_array.at(i));
					hover_info.last_frame++;
				}
			}
			else
			{
				hover_info.tex_array.push_back(nullptr);
				hover_info.last_frame++;
			}
			imagesVector->GetAnotherNode();
		}
	}

	if (to_load->GetBoolean("HasAnimatedClickedImages")) {
		JSONArraypack* imagesVector = to_load->GetArray("AnimatedClickedImages");
		for (int i = 0; i < imagesVector->GetArraySize(); ++i) {
			u64 textureID = std::stoull(imagesVector->GetString(std::to_string(i).data()));
			if (textureID != 0) {
				ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(textureID);
				if (tex != nullptr) {
					clicked_info.tex_array.push_back(nullptr);
					clicked_info.tex_array.at(i) = SetTextureArray(tex, clicked_info.tex_array.at(i));
					clicked_info.last_frame++;
				}
			}
			else
			{
				clicked_info.tex_array.push_back(nullptr);
				clicked_info.last_frame++;
			}
			imagesVector->GetAnotherNode();
		}
	}

	if (to_load->GetBoolean("HasAnimatedPressedImages")) {
		JSONArraypack* imagesVector = to_load->GetArray("AnimatedPressedImages");
		for (int i = 0; i < imagesVector->GetArraySize(); ++i) {
			u64 textureID = std::stoull(imagesVector->GetString(std::to_string(i).data()));
			if (textureID != 0) {
				ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(textureID);
				if (tex != nullptr) {
					pressed_info.tex_array.push_back(nullptr);
					pressed_info.tex_array.at(i) = SetTextureArray(tex, pressed_info.tex_array.at(i));
					pressed_info.last_frame++;
				}
			}
			else
			{
				pressed_info.tex_array.push_back(nullptr);
				pressed_info.last_frame++;
			}
			imagesVector->GetAnotherNode();
		}
	}

	if (to_load->GetBoolean("HasAnimatedDisabledImages")) {
		JSONArraypack* imagesVector = to_load->GetArray("AnimatedDisabledImages");
		for (int i = 0; i < imagesVector->GetArraySize(); ++i) {
			u64 textureID = std::stoull(imagesVector->GetString(std::to_string(i).data()));
			if (textureID != 0) {
				ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(textureID);
				if (tex != nullptr) {
					disabled_info.tex_array.push_back(nullptr);
					disabled_info.tex_array.at(i) = SetTextureArray(tex, disabled_info.tex_array.at(i));
					disabled_info.last_frame++;
				}
			}
			else
			{
				disabled_info.tex_array.push_back(nullptr);
				disabled_info.last_frame++;
			}
			imagesVector->GetAnotherNode();
		}
	}

	//-------------------------------------------------------------
	//-------------------------------------------------------------
	//-------------------------------------------------------------
	if (to_load->GetBoolean("HasListenersOnClick")) {
		JSONArraypack* onClickListeners = to_load->GetArray("ListenersOnClick");
		for (int i = 0; i < onClickListeners->GetArraySize(); ++i) {
			std::pair<std::string, std::function<void()>> pair = { onClickListeners->GetString(std::to_string(i).data()), std::function<void()>() };
			listenersOnClick.push_back(pair);
			onClickListeners->GetAnotherNode();
		}
	}

	if (to_load->GetBoolean("HasListenersOnHover")) {
		JSONArraypack* onHoverListeners = to_load->GetArray("ListenersOnHover");
		for (int i = 0; i < onHoverListeners->GetArraySize(); ++i) {
			std::pair<std::string, std::function<void()>> pair = { onHoverListeners->GetString(std::to_string(i).data()), std::function<void()>() };
			listenersOnHover.push_back(pair);
			onHoverListeners->GetAnotherNode();
		}
	}

	if (to_load->GetBoolean("HasListenersOnClickRepeat")) {
		JSONArraypack* onClickRepeatListeners = to_load->GetArray("ListenersOnClickRepeat");
		for (int i = 0; i < onClickRepeatListeners->GetArraySize(); ++i) {
			std::pair<std::string, std::function<void()>> pair = { onClickRepeatListeners->GetString(std::to_string(i).data()), std::function<void()>() };
			listenersOnClickRepeat.push_back(pair);
			onClickRepeatListeners->GetAnotherNode();
		}
	}

	if (to_load->GetBoolean("HasListenersOnRelease")) {
		JSONArraypack* onReleaseListeners = to_load->GetArray("ListenersOnRelease");
		for (int i = 0; i < onReleaseListeners->GetArraySize(); ++i) {
			std::pair<std::string, std::function<void()>> pair = { onReleaseListeners->GetString(std::to_string(i).data()), std::function<void()>() };
			listenersOnRelease.push_back(pair);
			onReleaseListeners->GetAnotherNode();
		}
	}

	if (to_load->GetBoolean("HasListenersOnExit")) {
		JSONArraypack* onExitListeners = to_load->GetArray("ListenersOnExit");
		for (int i = 0; i < onExitListeners->GetArraySize(); ++i) {
			std::pair<std::string, std::function<void()>> pair = { onExitListeners->GetString(std::to_string(i).data()), std::function<void()>() };
			listenersOnExit.push_back(pair);
			onExitListeners->GetAnotherNode();
		}
	}

	if (to_load->GetBoolean("HasListenersOnEnter")) {
		JSONArraypack* onEnterListeners = to_load->GetArray("ListenersOnEnter");
		for (int i = 0; i < onEnterListeners->GetArraySize(); ++i) {
			std::pair<std::string, std::function<void()>> pair = { onEnterListeners->GetString(std::to_string(i).data()), std::function<void()>() };
			listenersOnEnter.push_back(pair);
			onEnterListeners->GetAnotherNode();
		}
	}
	//-------------------------------------------------------------


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

	App->objects->first_assigned_selected = false;
}

void ComponentButton::SetAnimSpeed(float speed, UIState type)
{
	switch (type)
	{
	case Idle: {
		idle_info.speed = speed;
		break; }
	case Hover: {
		hover_info.speed = speed;
		break; }
	case Click: {
		clicked_info.speed = speed;
		break; }
	case Pressed: {
		pressed_info.speed = speed;
		break; }
	case Disabled: {
		disabled_info.speed = speed;
		break; }
	default: {
		break; }
	}
}

float ComponentButton::GetAnimSpeed(UIState type)
{
	switch (type)
	{
	case Idle: {
		return idle_info.speed;
		break; }
	case Hover: {
		return hover_info.speed;
		break; }
	case Click: {
		return clicked_info.speed;
		break; }
	case Pressed: {
		return pressed_info.speed;
		break; }
	case Disabled: {
		return disabled_info.speed;
		break; }
	default: {
		break; }
	}
}

void ComponentButton::HandleAlienEvent(const AlienEvent& e)
{

	switch (e.type)
	{
	case AlienEventType::SCRIPT_DELETED: {
		if (App->objects->inHotReload) {
			break;
		}
		ComponentScript* script = (ComponentScript*)e.object;
		if (script->game_object_attached == game_object_attached)
		{
			//Delete on Click
			for (auto functs = script->functionMap.begin(); functs != script->functionMap.end(); ++functs)
			{
				for (auto item = listenersOnClick.begin(); item != listenersOnClick.end(); ++item) {
					if ((*item).first == (*functs).first)
					{
						listenersOnClick.erase(item);
						//delete this from listeners on Click
						break;
					}
				}

			}

			//Delete on Hover
			for (auto functs = script->functionMap.begin(); functs != script->functionMap.end(); ++functs)
			{
				for (auto item = listenersOnHover.begin(); item != listenersOnHover.end(); ++item) {
					if ((*item).first == (*functs).first)
					{
						listenersOnHover.erase(item);
						//delete this from listeners on Click

						break;
					}
				}
			}

			//Delete on pressed
			for (auto functs = script->functionMap.begin(); functs != script->functionMap.end(); ++functs)
			{
				for (auto item = listenersOnClickRepeat.begin(); item != listenersOnClickRepeat.end(); ++item) {
					if ((*item).first == (*functs).first)
					{
						listenersOnClickRepeat.erase(item);
						//delete this from listeners on Click
						break;
					}
				}

			}

			//delete on release
			for (auto functs = script->functionMap.begin(); functs != script->functionMap.end(); ++functs)
			{
				for (auto item = listenersOnRelease.begin(); item != listenersOnRelease.end(); ++item) {
					if ((*item).first == (*functs).first)
					{
						listenersOnRelease.erase(item);
						break;
					}
				}
			}

			//delete on exit
			for (auto functs = script->functionMap.begin(); functs != script->functionMap.end(); ++functs)
			{
				for (auto item = listenersOnExit.begin(); item != listenersOnExit.end(); ++item) {
					if ((*item).first == (*functs).first)
					{
						listenersOnExit.erase(item);
						break;
					}
				}
			}

			//delete on enter
			for (auto functs = script->functionMap.begin(); functs != script->functionMap.end(); ++functs)
			{
				for (auto item = listenersOnEnter.begin(); item != listenersOnEnter.end(); ++item) {
					if ((*item).first == (*functs).first)
					{
						listenersOnEnter.erase(item);
						break;
					}
				}
			}

		}
		break; }

	default: {
		break; }
	}
}

bool ComponentButton::DrawInspector()
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

	if (ImGui::CollapsingHeader("Button", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		RightClickMenu("Button");

		ImGui::Spacing();
		if (ImGui::TreeNode("Textures"))
		{
			if (ImGui::TreeNode("Idle Texture"))
			{
				ImGui::Text("Add Image");
				ImGui::SameLine(125);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.0F,0.65F,0.0F,1.0F });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.0F,0.8F,0.0F,1.0F });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.0F,0.95F,0.0F,1.0F });
				ImGui::PushID(666666280426);
				if (ImGui::Button("+"))
				{
					//images.reserve(images.size() + 1);
					idle_info.tex_array.push_back(nullptr);
					idle_info.last_frame++;
				}
				ImGui::PopID();
				ImGui::PopStyleColor(3);
				ImGui::SameLine(145);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0.0F,0.0F,1.0F });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0.0F,0.0F,1.0F });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0.0F,0.0F,1.0F });
				ImGui::PushID(8277743420);
				if (ImGui::Button("-"))
				{
					if (!idle_info.tex_array.empty())
					{
						auto item = idle_info.tex_array.begin() + idle_info.last_frame - 1;
						ClearTextureArray((*item));
						idle_info.tex_array.erase(item);
						idle_info.last_frame--;
					}
				}
				ImGui::PopID();
				ImGui::PopStyleColor(3);

				ImGui::Spacing();
				for (auto item = idle_info.tex_array.begin(); item != idle_info.tex_array.end(); ++item)
				{
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
					ImGui::Text("Texture");

					ImGui::SameLine(125);
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
						ImGui::PushID(std::distance(idle_info.tex_array.begin(), item) + 96389723);
						if (ImGui::Button("X") && (*item) != nullptr) {
							//ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
							(*item) = ClearTextureArray((*item));
						}
						ImGui::PopID();
						ImGui::PopStyleColor(3);
					}
				}
				ImGui::TreePop();
			}
			ImGui::Spacing(); ImGui::Spacing();

			if (ImGui::TreeNode("Hover Texture"))
			{
				ImGui::Text("Add Image");
				ImGui::SameLine(125);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.0F,0.65F,0.0F,1.0F });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.0F,0.8F,0.0F,1.0F });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.0F,0.95F,0.0F,1.0F });
				ImGui::PushID(234234290021);
				if (ImGui::Button("+"))
				{
					//images.reserve(images.size() + 1);
					hover_info.tex_array.push_back(nullptr);
					hover_info.last_frame++;
				}
				ImGui::PopID();
				ImGui::PopStyleColor(3);
				ImGui::SameLine(145);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0.0F,0.0F,1.0F });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0.0F,0.0F,1.0F });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0.0F,0.0F,1.0F });
				ImGui::PushID(5050129875);
				if (ImGui::Button("-"))
				{
					if (!hover_info.tex_array.empty())
					{
						auto item = hover_info.tex_array.begin() + hover_info.last_frame - 1;
						ClearTextureArray((*item));
						hover_info.tex_array.erase(item);
						hover_info.last_frame--;
					}
				}
				ImGui::PopID();
				ImGui::PopStyleColor(3);

				ImGui::Spacing();
				for (auto item = hover_info.tex_array.begin(); item != hover_info.tex_array.end(); ++item)
				{
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
					ImGui::Text("Texture");

					ImGui::SameLine(125);
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
						ImGui::PushID(std::distance(hover_info.tex_array.begin(), item) + 73719723);
						if (ImGui::Button("X") && (*item) != nullptr) {
							//ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
							(*item) = ClearTextureArray((*item));
						}
						ImGui::PopID();
						ImGui::PopStyleColor(3);
					}
				}
				ImGui::TreePop();
			}
			ImGui::Spacing(); ImGui::Spacing();

			if (ImGui::TreeNode("Clicked Texture"))
			{
				ImGui::Text("Add Image");
				ImGui::SameLine(125);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.0F,0.65F,0.0F,1.0F });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.0F,0.8F,0.0F,1.0F });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.0F,0.95F,0.0F,1.0F });
				ImGui::PushID(5729455411);
				if (ImGui::Button("+"))
				{
					//images.reserve(images.size() + 1);
					clicked_info.tex_array.push_back(nullptr);
					clicked_info.last_frame++;
				}
				ImGui::PopID();
				ImGui::PopStyleColor(3);
				ImGui::SameLine(145);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0.0F,0.0F,1.0F });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0.0F,0.0F,1.0F });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0.0F,0.0F,1.0F });
				ImGui::PushID(9909176664);
				if (ImGui::Button("-"))
				{
					if (!clicked_info.tex_array.empty())
					{
						auto item = clicked_info.tex_array.begin() + clicked_info.last_frame - 1;
						ClearTextureArray((*item));
						clicked_info.tex_array.erase(item);
						clicked_info.last_frame--;
					}
				}
				ImGui::PopID();
				ImGui::PopStyleColor(3);

				ImGui::Spacing();
				for (auto item = clicked_info.tex_array.begin(); item != clicked_info.tex_array.end(); ++item)
				{
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
					ImGui::Text("Texture");

					ImGui::SameLine(125);
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
						ImGui::PushID(std::distance(clicked_info.tex_array.begin(), item) + 98265921);
						if (ImGui::Button("X") && (*item) != nullptr) {
							//ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
							(*item) = ClearTextureArray((*item));
						}
						ImGui::PopID();
						ImGui::PopStyleColor(3);
					}
				}
				ImGui::TreePop();
			}
			ImGui::Spacing(); ImGui::Spacing();

			if (ImGui::TreeNode("Pressed Texture"))
			{
				ImGui::Text("Add Image");
				ImGui::SameLine(125);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.0F,0.65F,0.0F,1.0F });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.0F,0.8F,0.0F,1.0F });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.0F,0.95F,0.0F,1.0F });
				ImGui::PushID(100036861111);
				if (ImGui::Button("+"))
				{
					//images.reserve(images.size() + 1);
					pressed_info.tex_array.push_back(nullptr);
					pressed_info.last_frame++;
				}
				ImGui::PopID();
				ImGui::PopStyleColor(3);
				ImGui::SameLine(145);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0.0F,0.0F,1.0F });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0.0F,0.0F,1.0F });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0.0F,0.0F,1.0F });
				ImGui::PushID(92748565254856);
				if (ImGui::Button("-"))
				{
					if (!pressed_info.tex_array.empty())
					{
						auto item = pressed_info.tex_array.begin() + pressed_info.last_frame - 1;
						ClearTextureArray((*item));
						pressed_info.tex_array.erase(item);
						pressed_info.last_frame--;
					}
				}
				ImGui::PopID();
				ImGui::PopStyleColor(3);

				ImGui::Spacing();
				for (auto item = pressed_info.tex_array.begin(); item != pressed_info.tex_array.end(); ++item)
				{
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
					ImGui::Text("Texture");

					ImGui::SameLine(125);
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
						ImGui::PushID(std::distance(pressed_info.tex_array.begin(), item) + 63029176);
						if (ImGui::Button("X") && (*item) != nullptr) {
							//ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
							(*item) = ClearTextureArray((*item));
						}
						ImGui::PopID();
						ImGui::PopStyleColor(3);
					}
				}
				ImGui::TreePop();
			}

			ImGui::Spacing(); ImGui::Spacing();

			if (ImGui::TreeNode("Disabled Texture"))
			{
				ImGui::Text("Add Image");
				ImGui::SameLine(125);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.0F,0.65F,0.0F,1.0F });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.0F,0.8F,0.0F,1.0F });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.0F,0.95F,0.0F,1.0F });
				ImGui::PushID(5109476238);
				if (ImGui::Button("+"))
				{
					//images.reserve(images.size() + 1);
					disabled_info.tex_array.push_back(nullptr);
					disabled_info.last_frame++;
				}
				ImGui::PopID();
				ImGui::PopStyleColor(3);
				ImGui::SameLine(145);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0.0F,0.0F,1.0F });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0.0F,0.0F,1.0F });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0.0F,0.0F,1.0F });
				ImGui::PushID(200465018388);
				if (ImGui::Button("-"))
				{
					if (!disabled_info.tex_array.empty())
					{
						auto item = disabled_info.tex_array.begin() + disabled_info.last_frame - 1;
						ClearTextureArray((*item));
						disabled_info.tex_array.erase(item);
						disabled_info.last_frame--;
					}
				}
				ImGui::PopID();
				ImGui::PopStyleColor(3);

				ImGui::Spacing();
				for (auto item = disabled_info.tex_array.begin(); item != disabled_info.tex_array.end(); ++item)
				{
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
					ImGui::Text("Texture");

					ImGui::SameLine(125);
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
						ImGui::PushID(std::distance(disabled_info.tex_array.begin(), item) + 732617365);
						if (ImGui::Button("X") && (*item) != nullptr) {
							//ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
							(*item) = ClearTextureArray((*item));
						}
						ImGui::PopID();
						ImGui::PopStyleColor(3);
					}
				}
				ImGui::TreePop();
			}
			ImGui::Spacing(); ImGui::Spacing();

			ImGui::TreePop();

		}
		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

		if (ImGui::TreeNode("Animation Settings")) {

			if (ImGui::TreeNode("Idle Settings")) {

				ImGui::Text("Loop");
				ImGui::SameLine(125);
				ImGui::Checkbox("##Loop", &idle_info.loop);
				ImGui::Spacing();
				ImGui::Text("Speed");
				ImGui::SameLine(125);
				ImGui::DragFloat("##Speed", &idle_info.speed, 0.5F, 0.1f, 100.0f, "%.1f");

				ImGui::Spacing();
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Hover Settings")) {

				ImGui::Text("Loop");
				ImGui::SameLine(125);
				ImGui::Checkbox("##Loop", &hover_info.loop);
				ImGui::Spacing();
				ImGui::Text("Speed");
				ImGui::SameLine(125);
				ImGui::DragFloat("##Speed", &hover_info.speed, 0.5F, 0.1f, 100.0f, "%.1f");

				ImGui::Spacing();
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Clicked Settings")) {

				ImGui::Text("Loop");
				ImGui::SameLine(125);
				ImGui::Checkbox("##Loop", &clicked_info.loop);
				ImGui::Spacing();
				ImGui::Text("Speed");
				ImGui::SameLine(125);
				ImGui::DragFloat("##Speed", &clicked_info.speed, 0.5F, 0.1f, 100.0f, "%.1f");

				ImGui::Spacing();
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Pressed Settings")) {

				ImGui::Text("Loop");
				ImGui::SameLine(125);
				ImGui::Checkbox("##Loop", &pressed_info.loop);
				ImGui::Spacing();
				ImGui::Text("Speed");
				ImGui::SameLine(125);
				ImGui::DragFloat("##Speed", &pressed_info.speed, 0.5F, 0.1f, 100.0f, "%.1f");

				ImGui::Spacing();
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Disabled Settings")) {

				ImGui::Text("Loop");
				ImGui::SameLine(125);
				ImGui::Checkbox("##Loop", &disabled_info.loop);
				ImGui::Spacing();
				ImGui::Text("Speed");
				ImGui::SameLine(125);
				ImGui::DragFloat("##Speed", &disabled_info.speed, 0.5F, 0.1f, 100.0f, "%.1f");

				ImGui::Spacing();
				ImGui::TreePop();
			}
			

			ImGui::TreePop();
		}

		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

		//------------------------SCRIPTS----------------------------
		if (ImGui::TreeNode("Script Listeners")) {
			//--------------
			if (ImGui::TreeNode("Functions Added")) {
				if (ImGui::TreeNode("On Click Added")) {
					for (auto item = listenersOnClick.begin(); item != listenersOnClick.end(); ++item) {

						ImGui::Text((*item).first.data());

						ImGui::SameLine();
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
						ImGui::PushID(std::distance(listenersOnClick.begin(), item) + 3876987);
						if (ImGui::Button("X") && (*item).second != nullptr) {
							listenersOnClick.erase(item);
							ImGui::PopID();
							ImGui::PopStyleColor(3);
							break;
							//delete function
						}
						ImGui::PopID();
						ImGui::PopStyleColor(3);
					}

					ImGui::TreePop();
				}
				ImGui::Spacing();
				if (ImGui::TreeNode("On Hover Added")) {
					for (auto item = listenersOnHover.begin(); item != listenersOnHover.end(); ++item) {

						ImGui::Text((*item).first.data());

						ImGui::SameLine();
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
						ImGui::PushID(std::distance(listenersOnHover.begin(), item) + 58613446);
						if (ImGui::Button("X") && (*item).second != nullptr) {
							listenersOnHover.erase(item);
							ImGui::PopID();
							ImGui::PopStyleColor(3);
							break;
							//delete function
						}
						ImGui::PopID();
						ImGui::PopStyleColor(3);

					}

					ImGui::TreePop();
				}
				ImGui::Spacing();
				if (ImGui::TreeNode("On Pressed Added")) {
					for (auto item = listenersOnClickRepeat.begin(); item != listenersOnClickRepeat.end(); ++item) {

						ImGui::Text((*item).first.data());

						ImGui::SameLine();
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
						ImGui::PushID(std::distance(listenersOnClickRepeat.begin(), item) + 4987543);
						if (ImGui::Button("X") && (*item).second != nullptr) {
							//delete function
							listenersOnClickRepeat.erase(item);
							ImGui::PopID();
							ImGui::PopStyleColor(3);
							break;
						}
						ImGui::PopID();
						ImGui::PopStyleColor(3);
					}

					ImGui::TreePop();
				}
				ImGui::Spacing();
				if (ImGui::TreeNode("On Release Added")) {
					for (auto item = listenersOnRelease.begin(); item != listenersOnRelease.end(); ++item) {

						ImGui::Text((*item).first.data());

						ImGui::SameLine();
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
						ImGui::PushID(std::distance(listenersOnRelease.begin(), item) + 7654367);
						if (ImGui::Button("X") && (*item).second != nullptr) {
							//delete function
							listenersOnRelease.erase(item);
							ImGui::PopID();
							ImGui::PopStyleColor(3);
							break;
						}
						ImGui::PopID();
						ImGui::PopStyleColor(3);
					}

					ImGui::TreePop();
				}
				ImGui::Spacing();
				if (ImGui::TreeNode("On Exit Added")) {
					for (auto item = listenersOnExit.begin(); item != listenersOnExit.end(); ++item) {

						ImGui::Text((*item).first.data());

						ImGui::SameLine();
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
						ImGui::PushID(std::distance(listenersOnExit.begin(), item) + 3812667);
						if (ImGui::Button("X") && (*item).second != nullptr) {
							//delete function
							listenersOnExit.erase(item);
							ImGui::PopID();
							ImGui::PopStyleColor(3);
							break;
						}
						ImGui::PopID();
						ImGui::PopStyleColor(3);
					}

					ImGui::TreePop();
				}

				ImGui::Spacing();
				if (ImGui::TreeNode("On Enter Added")) {
					for (auto item = listenersOnEnter.begin(); item != listenersOnEnter.end(); ++item) {

						ImGui::Text((*item).first.data());

						ImGui::SameLine();
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3);
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
						ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
						ImGui::PushID(std::distance(listenersOnEnter.begin(), item) + 1919751);
						if (ImGui::Button("X") && (*item).second != nullptr) {
							//delete function
							listenersOnEnter.erase(item);
							ImGui::PopID();
							ImGui::PopStyleColor(3);
							break;
						}
						ImGui::PopID();
						ImGui::PopStyleColor(3);
					}

					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
			//--------------

			ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

			std::vector<ComponentScript*> scripts = game_object_attached->GetComponents<ComponentScript>();
			if (ImGui::TreeNode("Functions To Add")) {
				if (!scripts.empty()) {
					if (ImGui::TreeNode("On Click To Add")) {
						for (auto item = scripts.begin(); item != scripts.end(); ++item) {
							if (*item != nullptr && (*item)->data_ptr != nullptr) {
								if (ImGui::BeginMenu((*item)->data_name.data())) {
									if (!(*item)->functionMap.empty()) {
										for (auto functs = (*item)->functionMap.begin(); functs != (*item)->functionMap.end(); ++functs) {
											if (ImGui::MenuItem((*functs).first.data())) {
												AddListenerOnClick((*functs).first, (*functs).second);
											}
										}
									}
									else {
										ImGui::Text("No exported functions");
									}
									ImGui::EndMenu();
								}
							}
						}
						ImGui::TreePop();
					}
					ImGui::Spacing();
					//-----------------------------
					if (ImGui::TreeNode("On Hover To Add")) {
						for (auto item = scripts.begin(); item != scripts.end(); ++item) {
							if (*item != nullptr && (*item)->data_ptr != nullptr) {
								if (ImGui::BeginMenu((*item)->data_name.data())) {
									if (!(*item)->functionMap.empty()) {
										for (auto functs = (*item)->functionMap.begin(); functs != (*item)->functionMap.end(); ++functs) {
											if (ImGui::MenuItem((*functs).first.data())) {
												AddListenerOnHover((*functs).first, (*functs).second);
											}
										}
									}
									else {
										ImGui::Text("No exported functions");
									}
									ImGui::EndMenu();
								}
							}
						}
						ImGui::TreePop();
					}
					ImGui::Spacing();
					//-----------------------------
					if (ImGui::TreeNode("On Pressed To Add")) {
						for (auto item = scripts.begin(); item != scripts.end(); ++item) {
							if (*item != nullptr && (*item)->data_ptr != nullptr) {
								if (ImGui::BeginMenu((*item)->data_name.data())) {
									if (!(*item)->functionMap.empty()) {
										for (auto functs = (*item)->functionMap.begin(); functs != (*item)->functionMap.end(); ++functs) {
											if (ImGui::MenuItem((*functs).first.data())) {
												AddListenerOnClickRepeat((*functs).first, (*functs).second);
											}
										}
									}
									else {
										ImGui::Text("No exported functions");
									}
									ImGui::EndMenu();
								}
							}
						}
						ImGui::TreePop();
					}
					ImGui::Spacing();
					//-----------------------------
					if (ImGui::TreeNode("On Release To Add")) {
						for (auto item = scripts.begin(); item != scripts.end(); ++item) {
							if (*item != nullptr && (*item)->data_ptr != nullptr) {
								if (ImGui::BeginMenu((*item)->data_name.data())) {
									if (!(*item)->functionMap.empty()) {
										for (auto functs = (*item)->functionMap.begin(); functs != (*item)->functionMap.end(); ++functs) {
											if (ImGui::MenuItem((*functs).first.data())) {
												AddListenerOnRelease((*functs).first, (*functs).second);
											}
										}
									}
									else {
										ImGui::Text("No exported functions");
									}
									ImGui::EndMenu();
								}
							}
						}
						ImGui::TreePop();
					}
					ImGui::Spacing();
					//-----------------------------
					if (ImGui::TreeNode("On Exit To Add")) {
						for (auto item = scripts.begin(); item != scripts.end(); ++item) {
							if (*item != nullptr && (*item)->data_ptr != nullptr) {
								if (ImGui::BeginMenu((*item)->data_name.data())) {
									if (!(*item)->functionMap.empty()) {
										for (auto functs = (*item)->functionMap.begin(); functs != (*item)->functionMap.end(); ++functs) {
											if (ImGui::MenuItem((*functs).first.data())) {
												AddListenerOnExit((*functs).first, (*functs).second);
											}
										}
									}
									else {
										ImGui::Text("No exported functions");
									}
									ImGui::EndMenu();
								}
							}
						}
						ImGui::TreePop();
					}

					ImGui::Spacing();
					//-----------------------------
					if (ImGui::TreeNode("On Enter To Add")) {
						for (auto item = scripts.begin(); item != scripts.end(); ++item) {
							if (*item != nullptr && (*item)->data_ptr != nullptr) {
								if (ImGui::BeginMenu((*item)->data_name.data())) {
									if (!(*item)->functionMap.empty()) {
										for (auto functs = (*item)->functionMap.begin(); functs != (*item)->functionMap.end(); ++functs) {
											if (ImGui::MenuItem((*functs).first.data())) {
												AddListenerOnEnter((*functs).first, (*functs).second);
											}
										}
									}
									else {
										ImGui::Text("No exported functions");
									}
									ImGui::EndMenu();
								}
							}
						}
						ImGui::TreePop();
					}
				}

				else {
					ImGui::Text("No Scripts attached");
				}
				ImGui::TreePop();
			}


			ImGui::TreePop();
		}
		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
		//------------------------COLOR-------------------------------
		if (ImGui::TreeNode("Colors"))
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Idle Color");
			ImGui::SameLine(140);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			if (ImGui::ColorEdit4("##RendererColorIdle", &idle_color, ImGuiColorEditFlags_Float)) {
				current_color = idle_color;
			}

			ImGui::Spacing();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Hover Color");
			ImGui::SameLine(140);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			if (ImGui::ColorEdit4("##RendererColorHover", &hover_color, ImGuiColorEditFlags_Float)) {

			}

			ImGui::Spacing();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Click Color");
			ImGui::SameLine(140);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			if (ImGui::ColorEdit4("##RendererColorClick", &clicked_color, ImGuiColorEditFlags_Float)) {

			}
			ImGui::Spacing();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Pressed Color");
			ImGui::SameLine(140);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			if (ImGui::ColorEdit4("##RendererColorPressed", &pressed_color, ImGuiColorEditFlags_Float)) {

			}
			ImGui::Spacing();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Disabled Color");
			ImGui::SameLine(140);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			if (ImGui::ColorEdit4("##RendererColorDisabled", &disabled_color, ImGuiColorEditFlags_Float)) {

			}

			ImGui::TreePop();
		}

		//---------------------END COLOR-----------------------------
		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

		if (ImGui::TreeNode("Navigation"))
		{
			//--------------------UP-----------------------------
			ImGui::Text("Select on Up");
			ImGui::SameLine(140);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.16f, 0.29F, 0.5, 1 });
			if (App->objects->GetGameObjectByID(select_on_up) != nullptr)
				ImGui::Button((select_on_up != -1) ? std::string((App->objects->GetGameObjectByID(select_on_up)->name)).data() : "GameObject: NULL", { ImGui::GetWindowWidth() * 0.55F , 0 });
			else
				ImGui::Button("GameObject: NULL", { ImGui::GetWindowWidth() * 0.55F , 0 });
			ImGui::PopStyleColor(3);
			if (ImGui::BeginDragDropTarget()) {
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_HIERARCHY_NODES, ImGuiDragDropFlags_SourceNoDisableHover);
				if (payload != nullptr && payload->IsDataType(DROP_ID_HIERARCHY_NODES)) {
					GameObject* obj = *(GameObject**)payload->Data;
					if (obj != nullptr && obj->GetComponent<ComponentUI>()->tabbable) {
						select_on_up = obj->ID;
					}
					else {
						LOG_ENGINE("Item is null or non tabbable");
					}
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::PushID(select_on_up);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
			if (ImGui::Button("X##selectUpBut")) {
				if (select_on_up != -1) {
					select_on_up = -1;
				}
			}
			ImGui::PopStyleColor(3);
			ImGui::Spacing();

			//--------------------DOWN-----------------------------
			ImGui::Text("Select on Down");
			ImGui::SameLine(140);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.16f, 0.29F, 0.5, 1 });
			if (App->objects->GetGameObjectByID(select_on_down) != nullptr)
				ImGui::Button((select_on_down != -1) ? std::string((App->objects->GetGameObjectByID(select_on_down)->name)).data() : "GameObject: NULL", { ImGui::GetWindowWidth() * 0.55F , 0 });
			else
				ImGui::Button("GameObject: NULL", { ImGui::GetWindowWidth() * 0.55F , 0 });
			ImGui::PopStyleColor(3);
			if (ImGui::BeginDragDropTarget()) {
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_HIERARCHY_NODES, ImGuiDragDropFlags_SourceNoDisableHover);
				if (payload != nullptr && payload->IsDataType(DROP_ID_HIERARCHY_NODES)) {
					GameObject* obj = *(GameObject**)payload->Data;
					if (obj != nullptr && obj->GetComponent<ComponentUI>()->tabbable) {
						select_on_down = obj->ID;
					}
					else {
						LOG_ENGINE("Item is null or non tabbable");
					}
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::PushID(select_on_down);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
			if (ImGui::Button("X##selectDownBut")) {
				if (select_on_down != -1) {
					select_on_down = -1;
				}
			}
			ImGui::PopStyleColor(3);




			ImGui::Spacing();

			//--------------------RIGHT-----------------------------
			ImGui::Text("Select on Right");
			ImGui::SameLine(140);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.16f, 0.29F, 0.5, 1 });
			if (App->objects->GetGameObjectByID(select_on_right) != nullptr)
				ImGui::Button((select_on_right != -1) ? std::string((App->objects->GetGameObjectByID(select_on_right)->name)).data() : "GameObject: NULL", { ImGui::GetWindowWidth() * 0.55F , 0 });
			else
				ImGui::Button("GameObject: NULL", { ImGui::GetWindowWidth() * 0.55F , 0 });
			ImGui::PopStyleColor(3);
			if (ImGui::BeginDragDropTarget()) {
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_HIERARCHY_NODES, ImGuiDragDropFlags_SourceNoDisableHover);
				if (payload != nullptr && payload->IsDataType(DROP_ID_HIERARCHY_NODES)) {
					GameObject* obj = *(GameObject**)payload->Data;
					if (obj != nullptr && obj->GetComponent<ComponentUI>()->tabbable) {
						select_on_right = obj->ID;
					}
					else {
						LOG_ENGINE("Item is null or non tabbable");
					}
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::PushID(select_on_right);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
			if (ImGui::Button("X##selectRightBut")) {
				if (select_on_right != -1) {
					select_on_right = -1;
				}
			}
			ImGui::PopStyleColor(3);





			ImGui::Spacing();

			//--------------------LEFT-----------------------------
			ImGui::Text("Select on Left");
			ImGui::SameLine(140);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.16f, 0.29F, 0.5, 1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.16f, 0.29F, 0.5, 1 });
			if (App->objects->GetGameObjectByID(select_on_left) != nullptr)
				ImGui::Button((select_on_left != -1) ? std::string((App->objects->GetGameObjectByID(select_on_left)->name)).data() : "GameObject: NULL", { ImGui::GetWindowWidth() * 0.55F , 0 });
			else
				ImGui::Button("GameObject: NULL", { ImGui::GetWindowWidth() * 0.55F , 0 });
			ImGui::PopStyleColor(3);
			if (ImGui::BeginDragDropTarget()) {
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_HIERARCHY_NODES, ImGuiDragDropFlags_SourceNoDisableHover);
				if (payload != nullptr && payload->IsDataType(DROP_ID_HIERARCHY_NODES)) {
					GameObject* obj = *(GameObject**)payload->Data;
					if (obj != nullptr && obj->GetComponent<ComponentUI>()->tabbable) {
						select_on_left = obj->ID;
					}
					else {
						LOG_ENGINE("Item is null or non tabbable");
					}
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::PushID(select_on_left);
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
			if (ImGui::Button("X##selectLeftBut")) {
				if (select_on_left != -1) {
					select_on_left = -1;
				}
			}
			ImGui::PopStyleColor(3);

			ImGui::Spacing();
			//----------------------------------------------------------------------

			ImGui::TreePop();
		}

		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

		if (ImGui::TreeNode("Audio Events"))
		{
			ImGui::Text("Move Event");
			ImGui::SameLine(120);

			static char move_name[30];
			memcpy(move_name, move_event.c_str(), 30);

			if (ImGui::InputText("##MoveEventName", move_name, 30, ImGuiInputTextFlags_AutoSelectAll))
			{
				move_event = move_name;
			}
			ImGui::Spacing();

			ImGui::Text("Click Event");
			ImGui::SameLine(120);

			static char click_name[30];
			memcpy(click_name, click_event.c_str(), 30);

			if (ImGui::InputText("##ClickEventName", click_name, 30, ImGuiInputTextFlags_AutoSelectAll))
			{
				click_event = click_name;
			}
			ImGui::Spacing(); ImGui::Spacing();



			ImGui::TreePop();
		}

		ImGui::Separator();
		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
	}
	else {
		RightClickMenu("Button");
	}

	return true;
}


void ComponentButton::Draw(bool isGame)
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

	if (!current_tex_array->tex_array.empty()) {
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

		/*if (game_object_attached->parent->GetComponent<ComponentCanvas>() != nullptr)
		{
			float4x4 parentGlobal = canvas->GetGlobalTransform();
			uiGlobal = parentGlobal * uiLocal;
		}*/

		glPushMatrix();
		glMultMatrixf(uiGlobal.Transposed().ptr());

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


bool ComponentButton::OnEnter()
{
	if (active)
	{
		ComponentAudioEmitter* emitter = game_object_attached->GetComponent<ComponentAudioEmitter>();
		if (emitter != nullptr)
		{
			emitter->StartSound(move_event.c_str());
		}
		CallListeners(&listenersOnEnter);
	}
	return true;
}

bool ComponentButton::OnIdle()
{
	if (active) {
		current_color = idle_color;

		SetCurrentTexArray(&idle_info);
		
	}
	return true;
}

bool ComponentButton::OnHover()
{
	if (active) {

		current_color = hover_color;
		
		SetCurrentTexArray(&hover_info);
		
		CallListeners(&listenersOnHover);
	}
	return true;
}

bool ComponentButton::OnClick()
{
	if (active) {

		ComponentAudioEmitter* emitter = game_object_attached->GetComponent<ComponentAudioEmitter>();
		if (emitter != nullptr)
		{
			emitter->StartSound(click_event.c_str());
		}
		
		current_color = clicked_color;
		
		SetCurrentTexArray(&clicked_info);
		
		CallListeners(&listenersOnClick);
	}
	return true;
}

bool ComponentButton::OnPressed()
{
	if (active) {
		current_color = pressed_color;
		
		SetCurrentTexArray(&pressed_info);
		
		CallListeners(&listenersOnClickRepeat);
	}
	return true;
}

bool ComponentButton::OnRelease()
{
	if (active) {
		pressed_info.current_frame = 0.0f;
		CallListeners(&listenersOnRelease);
	}
	return true;
}

bool ComponentButton::OnExit()
{
	if (active)
	{

		CallListeners(&listenersOnExit);
	}
	return true;
}

void ComponentButton::CallListeners(std::vector<std::pair<std::string, std::function<void()>>>* listeners)
{
	if (listeners != nullptr) {
		auto item = listeners->begin();
		for (; item != listeners->end(); ++item) {
			if ((*item).second != nullptr) {
				try {
					(*item).second();
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


ResourceTexture* ComponentButton::ClearTextureArray(ResourceTexture* item)
{
	if (item != nullptr) {
		item->DecreaseReferences();
		item = nullptr;
		return nullptr;
	}
	return nullptr;
}

ResourceTexture* ComponentButton::SetTextureArray(ResourceTexture* tex, ResourceTexture* item)
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

void ComponentButton::SetCurrentTexArray(AnimationInfo* new_tex)
{
	for (auto item = current_tex_array->tex_array.begin(); item != current_tex_array->tex_array.end(); ++item)
	{
		if ((*item) != nullptr) {
			(*item)->DecreaseReferences();
		}
	}
	
	current_tex_array = new_tex;
	if (!new_tex->tex_array.empty())
	{
		for (auto item = current_tex_array->tex_array.begin(); item != current_tex_array->tex_array.end(); ++item)
		{
			if ((*item) != nullptr) {
				(*item)->IncreaseReferences();
				SetSize((*item)->width, (*item)->height);
			}
		}
	}
	
}

ResourceTexture* ComponentButton::GetCurrentFrame(float dt)
{
	current_tex_array->current_frame += current_tex_array->speed * dt;
	if (current_tex_array->current_frame >= current_tex_array->last_frame)
	{
		current_tex_array->current_frame = (current_tex_array->loop) ? 0.0f : current_tex_array->last_frame - 1;
		current_tex_array->loops++;
	}
	return current_tex_array->tex_array.at((int)current_tex_array->current_frame);
}

bool ComponentButton::Finished() const
{
	return current_tex_array->loops > 0;
}

void ComponentButton::Reset()
{
	current_tex_array->loops = 0;
	current_tex_array->current_frame = 0.0f;

}

int ComponentButton::SeeCurrentFrame()
{
	return (int)current_tex_array->current_frame;
}


void ComponentButton::SetActive(bool active)
{
	this->active_ui = active;
	if (active_ui) {
		current_color = idle_color;

		if (!idle_info.tex_array.empty())
		{
			SetCurrentTexArray(&idle_info);
		}
	}
	else {
		current_color = disabled_color;
		if (!disabled_info.tex_array.empty())
		{
			SetCurrentTexArray(&disabled_info);
		}
	}
}

void ComponentButton::AddListenerOnHover(std::string name, std::function<void()> funct)
{
	std::pair<std::string, std::function<void()>> pair = { name, funct };
	if (!CheckIfScriptIsAlreadyAdded(&listenersOnHover, name))
	{
		listenersOnHover.push_back(pair);
	}
}

void ComponentButton::AddListenerOnClick(std::string name, std::function<void()> funct)
{
	std::pair<std::string, std::function<void()>> pair = { name, funct };
	if (!CheckIfScriptIsAlreadyAdded(&listenersOnClick, name))
	{
		listenersOnClick.push_back(pair);
	}
}

void ComponentButton::AddListenerOnClickRepeat(std::string name, std::function<void()> funct)
{
	std::pair<std::string, std::function<void()>> pair = { name, funct };
	if (!CheckIfScriptIsAlreadyAdded(&listenersOnClickRepeat, name))
	{
		listenersOnClickRepeat.push_back(pair);
	}
}

void ComponentButton::AddListenerOnRelease(std::string name, std::function<void()> funct)
{
	std::pair<std::string, std::function<void()>> pair = { name, funct };
	if (!CheckIfScriptIsAlreadyAdded(&listenersOnRelease, name))
	{
		listenersOnRelease.push_back(pair);
	}
}

void ComponentButton::AddListenerOnExit(std::string name, std::function<void()> funct)
{
	std::pair<std::string, std::function<void()>> pair = { name, funct };
	if (!CheckIfScriptIsAlreadyAdded(&listenersOnExit, name))
	{
		listenersOnExit.push_back(pair);
	}
}

void ComponentButton::AddListenerOnEnter(std::string name, std::function<void()> funct)
{
	std::pair<std::string, std::function<void()>> pair = { name, funct };
	if (!CheckIfScriptIsAlreadyAdded(&listenersOnEnter, name))
	{
		listenersOnEnter.push_back(pair);
	}
}

bool ComponentButton::CheckIfScriptIsAlreadyAdded(std::vector<std::pair<std::string, std::function<void()>>>* listeners, const std::string& name)
{
	if (listeners != nullptr) {

		for (auto item = listeners->begin(); item != listeners->end(); ++item) {

			if ((*item).first == name)
				return true;
		}
	}
	return false;
}