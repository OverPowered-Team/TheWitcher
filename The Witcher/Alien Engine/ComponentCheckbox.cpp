#include "ComponentCheckbox.h"
#include "ComponentCanvas.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include "ComponentImage.h"
#include "ComponentTransform.h"
#include "ComponentAudioEmitter.h"
#include "GameObject.h"
#include "imgui/imgui.h"
#include "ComponentUI.h"
#include "ReturnZ.h"
#include "ModuleWindow.h"
#include "PanelGame.h"
#include "ResourceTexture.h"
#include "ComponentScript.h"
#include "Application.h"
#include "PanelProject.h"
#include "ModuleResources.h"
#include "ModuleUI.h"
#include "ModuleRenderer3D.h"
#include "ComponentTransform.h"
#include "mmgr/mmgr.h"

ComponentCheckbox::ComponentCheckbox(GameObject* obj) : ComponentUI(obj)
{
	ui_type = ComponentType::UI_CHECKBOX;
	tabbable = true;
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
			if (ImGui::Button("X##CheckCrossTex") && crossTexture != nullptr) {
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
			if (ImGui::Button("X##CheckTickTex") && tickTexture != nullptr) {
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
						ImGui::PushID(std::distance(listenersOnClick.begin(), item) + 312387);
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
						ImGui::PushID(std::distance(listenersOnHover.begin(), item) + 7867246);
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
						ImGui::PushID(std::distance(listenersOnClickRepeat.begin(), item) + 4987321);
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
						ImGui::PushID(std::distance(listenersOnRelease.begin(), item) + 7691967);
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
						ImGui::PushID(std::distance(listenersOnExit.begin(), item) + 700143);
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
						ImGui::PushID(std::distance(listenersOnEnter.begin(), item) + 1555722);
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
			//----------------------

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

		//------------------------COLOR BACKGROUND-------------------------------
		if (ImGui::TreeNode("Background Colors"))
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

		//---------------------END COLOR BACKGROUND-----------------------------

		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

		//------------------------COLOR CHECKBOX-------------------------------
		if (ImGui::TreeNode("Checkbox Colors"))
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Idle Color");
			ImGui::SameLine(140);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			if (ImGui::ColorEdit4("##RendererColorIdle", &checkbox_idle_color, ImGuiColorEditFlags_Float)) {
				checkbox_current_color = checkbox_idle_color;
			}

			ImGui::Spacing();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Hover Color");
			ImGui::SameLine(140);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			ImGui::ColorEdit4("##RendererColorHover", &checkbox_hover_color, ImGuiColorEditFlags_Float);



			ImGui::Spacing();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Click Color");
			ImGui::SameLine(140);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			ImGui::ColorEdit4("##RendererColorClick", &checkbox_clicked_color, ImGuiColorEditFlags_Float);


			ImGui::Spacing();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Pressed Color");
			ImGui::SameLine(140);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			ImGui::ColorEdit4("##RendererColorPressed", &checkbox_pressed_color, ImGuiColorEditFlags_Float);


			ImGui::Spacing();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Disabled Color");
			ImGui::SameLine(140);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			ImGui::ColorEdit4("##RendererColorDisabled", &checkbox_disabled_color, ImGuiColorEditFlags_Float);


			ImGui::TreePop();
		}

		//---------------------END COLOR CHECKBOX-----------------------------


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
			if (ImGui::Button("X##selectUpCheck")) {
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
			if (ImGui::Button("X##selectDownCheck")) {
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
			if (ImGui::Button("X##selectRightCheck")) {
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
			if (ImGui::Button("X##selectLeftCheck")) {
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
		DrawTexture(isGame, tickTexture, false);
	else
		DrawTexture(isGame, crossTexture, false);

	transform->global_transformation = matrix;
	DrawTexture(isGame, texture);
}


void ComponentCheckbox::DrawTexture(bool isGame, ResourceTexture* tex, bool background)
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

	if(background)
	glColor4f(current_color.r, current_color.g, current_color.b, current_color.a);
	else
		glColor4f(checkbox_current_color.r, checkbox_current_color.g, checkbox_current_color.b, checkbox_current_color.a);

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

bool ComponentCheckbox::OnIdle()
{
	if (active) {
		current_color = idle_color;
		checkbox_current_color = checkbox_idle_color;
	}
	return true;
}

bool ComponentCheckbox::OnHover()
{
	if (active)
	{
		current_color = hover_color;
		checkbox_current_color = checkbox_hover_color;
	}
	return true;
}

bool ComponentCheckbox::OnClick()
{
	if (active)
	{ 
		ComponentAudioEmitter* emitter = game_object_attached->GetComponent<ComponentAudioEmitter>();
		if (emitter != nullptr)
		{
			emitter->StartSound(click_event.c_str());
		}

		current_color = clicked_color;
		checkbox_current_color = checkbox_clicked_color;

		CallListeners(&listenersOnClick);
	}
	return true;
}

bool ComponentCheckbox::OnPressed()
{
	if (active)
	{
		current_color = pressed_color;
		checkbox_current_color = checkbox_pressed_color;
		CallListeners(&listenersOnClickRepeat);
	}
	return true;
}

bool ComponentCheckbox::OnRelease()
{
	if (active)
	{ 
		clicked = !clicked;
		current_color = hover_color;
		checkbox_current_color = checkbox_hover_color;
		CallListeners(&listenersOnRelease);
	}
	return true;
}

bool ComponentCheckbox::OnExit()
{
	if (active)
	{
		CallListeners(&listenersOnExit);
	}
	return true;
}

bool ComponentCheckbox::OnEnter()
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

void ComponentCheckbox::SetActive(bool active)
{
	this->active_ui = active;
	if (active) {
		current_color = idle_color;
		checkbox_current_color = checkbox_idle_color;
	}
	else {
		current_color = disabled_color;
		checkbox_current_color = checkbox_disabled_color;
	}
}

void ComponentCheckbox::AddListenerOnHover(std::string name, std::function<void()> funct)
{
	std::pair<std::string, std::function<void()>> pair = { name, funct };
	if (!CheckIfScriptIsAlreadyAdded(&listenersOnHover, name))
	{
		listenersOnHover.push_back(pair);
	}
}

void ComponentCheckbox::AddListenerOnClick(std::string name, std::function<void()> funct)
{
	std::pair<std::string, std::function<void()>> pair = { name, funct };
	if (!CheckIfScriptIsAlreadyAdded(&listenersOnClick, name))
	{
		listenersOnClick.push_back(pair);
	}
}

void ComponentCheckbox::AddListenerOnClickRepeat(std::string name, std::function<void()> funct)
{
	std::pair<std::string, std::function<void()>> pair = { name, funct };
	if (!CheckIfScriptIsAlreadyAdded(&listenersOnClickRepeat, name))
	{
		listenersOnClickRepeat.push_back(pair);
	}
}

void ComponentCheckbox::AddListenerOnRelease(std::string name, std::function<void()> funct)
{
	std::pair<std::string, std::function<void()>> pair = { name, funct };
	if (!CheckIfScriptIsAlreadyAdded(&listenersOnRelease, name))
	{
		listenersOnRelease.push_back(pair);
	}
}

void ComponentCheckbox::AddListenerOnExit(std::string name, std::function<void()> funct)
{
	std::pair<std::string, std::function<void()>> pair = { name, funct };
	if (!CheckIfScriptIsAlreadyAdded(&listenersOnExit, name))
	{
		listenersOnExit.push_back(pair);
	}
}

void ComponentCheckbox::AddListenerOnEnter(std::string name, std::function<void()> funct)
{
	std::pair<std::string, std::function<void()>> pair = { name, funct };
	if (!CheckIfScriptIsAlreadyAdded(&listenersOnEnter, name))
	{
		listenersOnEnter.push_back(pair);
	}
}

bool ComponentCheckbox::CheckIfScriptIsAlreadyAdded(std::vector<std::pair<std::string, std::function<void()>>>* listeners, const std::string& name)
{
	if (listeners != nullptr) {

		for (auto item = listeners->begin(); item != listeners->end(); ++item) {

			if ((*item).first == name)
				return true;
		}
	}
	return false;
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
	to_save->SetString("TextureID", (texture != nullptr) ? std::to_string(texture->GetID()).data() : "0");
	to_save->SetString("crossTexture", (crossTexture != nullptr) ? std::to_string(crossTexture->GetID()).data() : "0");
	to_save->SetString("tickTexture", (tickTexture != nullptr) ? std::to_string(tickTexture->GetID()).data() : "0");
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

	to_save->SetColor("CheckboxColorCurrent", checkbox_current_color);
	to_save->SetColor("CheckboxColorIdle", checkbox_idle_color);
	to_save->SetColor("CheckboxColorHover", checkbox_hover_color);
	to_save->SetColor("CheckboxColorClicked", checkbox_clicked_color);
	to_save->SetColor("CheckboxColorPressed", checkbox_pressed_color);
	to_save->SetColor("CheckboxColorDisabled", checkbox_disabled_color);

	to_save->SetString("SelectOnUp", std::to_string(select_on_up).data());
	to_save->SetString("SelectOnDown", std::to_string(select_on_down).data());
	to_save->SetString("SelectOnRight", std::to_string(select_on_right).data());
	to_save->SetString("SelectOnLeft", std::to_string(select_on_left).data());

	to_save->SetString("ClickEvent", click_event.data());
	to_save->SetString("MoveEvent", move_event.data());

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
}

void ComponentCheckbox::LoadComponent(JSONArraypack* to_load)
{
	x = to_load->GetNumber("X");
	y = to_load->GetNumber("Y");
	size = { (float)to_load->GetNumber("Width"), (float)to_load->GetNumber("Height") };

	enabled = to_load->GetBoolean("Enabled");
	clicked = to_load->GetBoolean("clicked");
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

	checkbox_current_color = to_load->GetColor("CheckboxColorCurrent");
	checkbox_idle_color = to_load->GetColor("CheckboxColorIdle");
	checkbox_hover_color = to_load->GetColor("CheckboxColorHover");
	checkbox_clicked_color = to_load->GetColor("CheckboxColorClicked");
	checkbox_pressed_color = to_load->GetColor("CheckboxColorPressed");
	checkbox_disabled_color = to_load->GetColor("CheckboxColorDisabled");

	select_on_up = std::stoull(to_load->GetString("SelectOnUp"));
	select_on_down = std::stoull(to_load->GetString("SelectOnDown"));
	select_on_right = std::stoull(to_load->GetString("SelectOnRight"));
	select_on_left = std::stoull(to_load->GetString("SelectOnLeft"));

	click_event = to_load->GetString("ClickEvent");
	move_event = to_load->GetString("MoveEvent");

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
	App->objects->first_assigned_selected = false;
}

bool ComponentCheckbox::GetStatus()
{
	return clicked;
}

void ComponentCheckbox::HandleAlienEvent(const AlienEvent& e)
{
	switch (e.type)
	{
	case AlienEventType::SCRIPT_DELETED: {
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

void ComponentCheckbox::CallListeners(std::vector<std::pair<std::string, std::function<void()>>>* listeners)
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

ComponentCanvas* ComponentCheckbox::GetCanvas()
{
	ComponentCanvas* canvas = App->objects->GetRoot(true)->GetCanvas();
	if (canvas == nullptr) {
		GameObject* obj = new GameObject(App->objects->GetRoot(false));
		obj->SetName("Canvas");
		canvas = new ComponentCanvas(obj);
		obj->AddComponent(canvas);
	}
	return canvas;
}
