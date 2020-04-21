#include "PanelSceneSelector.h"
#include "Color.h"
#include "imgui/imgui.h"
#include "ResourceTexture.h"
#include "imgui/imgui_internal.h"
#include "FileNode.h"
#include "ResourceScene.h"
#include "PanelProject.h"
#include "ShortCutManager.h"
#include "ModuleResources.h"
#include "GameObject.h"
#include "ModuleObjects.h"
#include "ModuleUI.h"
#include "mmgr/mmgr.h"

PanelSceneSelector::PanelSceneSelector(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat, const SDL_Scancode& key3_repeat_extra)
	: Panel(panel_name, key1_down, key2_repeat, key3_repeat_extra)
{
	shortcut = App->shortcut_manager->AddShortCut("Save", key1_down, std::bind(&PanelSceneSelector::OrganizeSaveScene, this), key2_repeat, key3_repeat_extra);
	enabled = false;
}

PanelSceneSelector::~PanelSceneSelector()
{
}

void PanelSceneSelector::PanelLogic()
{
	if (menu_save_current) {
		MenuSaveCurrentScene();
	}
}

void PanelSceneSelector::OrganizeSave(const SceneSelectorState& state)
{
	switch (state) {
	case SceneSelectorState::SAVE_SCENE: {
		OrganizeSaveScene();
		break; }
	case SceneSelectorState::SAVE_AS_NEW: {
		SaveSceneAsNew();
		break; }
	case SceneSelectorState::CREATE_NEW_SCENE: {
		OrganizeCreateNewScene();
		break; }
	case SceneSelectorState::LOAD_SCENE: {
		OrganizeLoad();
		break; }
	}
}

void PanelSceneSelector::OrganizeSaveScene()
{
	if (App->objects->current_scenes.empty()) {
		GameObject* root = App->objects->GetGlobalRoot();
		force_save = root->children.back();
		OrganizeSave(SceneSelectorState::SAVE_AS_NEW);
	}
	else {
		for (auto item = App->objects->current_scenes.begin(); item != App->objects->current_scenes.end(); ++item) {
			App->objects->SaveScene(*item);
		}
		GameObject* root = App->objects->GetGlobalRoot();
		for (auto item = root->children.begin(); item != root->children.end(); ++item) {
			if (strcmp((*item)->GetName(), "Untitled*") == 0) {
				force_save = *item;
				OrganizeSave(SceneSelectorState::SAVE_AS_NEW);
				break;
			}
		}
	}
}

void PanelSceneSelector::OrganizeCreateNewScene()
{
	menu_save_current = true;
	create_new = true;
}

void PanelSceneSelector::OrganizeLoad()
{
	menu_save_current = true;
	load = true;
}

void PanelSceneSelector::LoadSceneWithPath(const char* path)
{
	scene_to_load = std::string(path);
	menu_save_current = true;
}

void PanelSceneSelector::SaveSceneAsNew()
{
	OPENFILENAME to_save;

	static char filename[MAX_PATH];

	// get the current game directory
	static char curr_dir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, curr_dir);
	
	// add the local path of the scenes folder
	std::string dir = std::string(curr_dir + std::string("\\") + std::string("Assets\\Scenes")).data();
	
	// fill eveything with 0  in order to avoid problems
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&to_save, sizeof(to_save));

	to_save.lStructSize = sizeof(to_save);
	to_save.hwndOwner = NULL;  
	to_save.lpstrFilter = "alienScene\0*.alienScene";  
	to_save.lpstrFile = filename; 
	to_save.nMaxFile = MAX_PATH;
	to_save.lpstrTitle = "Save As New .alienScene"; 
	to_save.lpstrInitialDir = dir.data();
	to_save.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_OVERWRITEPROMPT;
	if (GetSaveFileNameA(&to_save)) 
	{	
		SetCurrentDirectoryA(curr_dir);

		std::string extension;
		App->file_system->SplitFilePath(filename, nullptr, nullptr, &extension);

		std::string path;
		if (!App->StringCmp("alienScene", extension.data())) {
			path = std::string(filename + std::string(".alienScene")).data();
		}
		else {
			path = filename;
		}

		std::string curr(curr_dir);
		App->file_system->NormalizePath(curr);
		App->file_system->NormalizePath(path);
		for (uint i = 0; i < curr.size(); ++i) {
			path.erase(path.begin());
		}
		path.erase(path.begin());
		if (App->file_system->Exists(path.data())) {
			ResourceScene* scene = App->resources->GetSceneByName(App->file_system->GetBaseFileName(path.data()).data());
			if (scene != nullptr) {
				scene->DeleteMetaData();
				remove(scene->GetAssetsPath());
				remove(std::string(App->file_system->GetPathWithoutExtension(scene->GetAssetsPath()) + "_meta.lazy").data());
				auto item = App->resources->resources.begin();
				for (; item != App->resources->resources.end(); ++item) {
					if (*item == scene) {
						delete* item;
						*item = nullptr;
						App->resources->resources.erase(item);
						break;
					}
				}
			}
		}

		ResourceScene* scene = new ResourceScene();
		scene->SetAssetsPath(path.data());
		scene->CreateMetaData();

		if (force_save == nullptr && App->objects->GetGlobalRoot()->children.size() == 1) {
			force_save = App->objects->GetGlobalRoot()->children.front();
		}

		if (force_save != nullptr) {
			force_save->ID = scene->GetID();
			force_save->SetName(scene->GetName());
			force_save = nullptr;
		}
		App->objects->SaveScene(scene);

		// last of all, refresh nodes because I have no idea if the user has created folders or moved things in the explorer. Users are bad people creating folders without using the alien engine explorer :(
		App->ui->panel_project->RefreshAllNodes();
	}
	else { 
		SetCurrentDirectoryA(curr_dir);
	}
	
}

void PanelSceneSelector::LoadScene()
{
	OPENFILENAME to_load;

	static char filename[MAX_PATH];

	// get the current game directory
	static char curr_dir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, curr_dir);

	// add the local path of the scenes folder
	std::string dir = std::string(curr_dir + std::string("\\") + std::string("Assets\\Scenes")).data();

	// fill eveything with 0  in order to avoid problems
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&to_load, sizeof(to_load));

	to_load.lStructSize = sizeof(to_load);
	to_load.hwndOwner = NULL;
	to_load.lpstrFilter = "alienScene\0*.alienScene";
	to_load.lpstrFile = filename;
	to_load.nMaxFile = MAX_PATH;
	to_load.lpstrTitle = "Load a .alienScene";
	to_load.lpstrInitialDir = dir.data();
	to_load.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_EXPLORER;
	if (GetOpenFileNameA(&to_load))
	{
		SetCurrentDirectoryA(curr_dir);
		std::string name = filename;
		App->file_system->NormalizePath(name);

		App->objects->LoadScene(App->file_system->GetBaseFileName(name.data()).data());

		// last of all, refresh nodes because I have no idea if the user has created folders or moved things in the explorer. Users are bad people creating folders without using the alien engine explorer :(
		App->ui->panel_project->RefreshAllNodes();
	}
	else {
		SetCurrentDirectoryA(curr_dir);
	}
}

void PanelSceneSelector::CreateNewScene()
{
	App->objects->CreateEmptyScene();
	//OPENFILENAME to_save;

	//static char filename[MAX_PATH];

	//// get the current game directory
	//static char curr_dir[MAX_PATH];
	//GetCurrentDirectoryA(MAX_PATH, curr_dir);

	//// add the local path of the scenes folder
	//std::string dir = std::string(curr_dir + std::string("\\") + std::string("Assets\\Scenes")).data();

	//// fill eveything with 0  in order to avoid problems
	//ZeroMemory(&filename, sizeof(filename));
	//ZeroMemory(&to_save, sizeof(to_save));

	//to_save.lStructSize = sizeof(to_save);
	//to_save.hwndOwner = NULL;
	//to_save.lpstrFilter = "alienScene\0*.alienScene";
	//to_save.lpstrFile = filename;
	//to_save.nMaxFile = MAX_PATH;
	//to_save.lpstrTitle = "Create a new .alienScene";
	//to_save.lpstrInitialDir = dir.data();
	//to_save.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_OVERWRITEPROMPT;
	//if (GetSaveFileNameA(&to_save))
	//{
	//	SetCurrentDirectoryA(curr_dir);

	//	std::string extension;
	//	App->file_system->SplitFilePath(filename, nullptr, nullptr, &extension);

	//	std::string path;
	//	if (!App->StringCmp("alienScene", extension.data())) {
	//		path = std::string(filename + std::string(".alienScene")).data();
	//	}
	//	else {
	//		path = filename;
	//	}
	//	std::string curr(curr_dir);
	//	App->file_system->NormalizePath(curr);
	//	App->file_system->NormalizePath(path);
	//	for (uint i = 0; i < curr.size(); ++i) {
	//		path.erase(path.begin());
	//	}
	//	path.erase(path.begin());
	//	
	//	if (App->file_system->Exists(path.data())) {
	//		ResourceScene* scene = App->resources->GetSceneByName(App->file_system->GetBaseFileName(path.data()).data());
	//		if (scene != nullptr) {
	//			scene->DeleteMetaData();
	//			remove(scene->GetAssetsPath());
	//			remove(std::string(App->file_system->GetPathWithoutExtension(scene->GetAssetsPath()) + "_meta.lazy").data());
	//			auto item = App->resources->resources.begin();
	//			for (; item != App->resources->resources.end(); ++item) {
	//				if (*item == scene) {
	//					delete* item;
	//					*item = nullptr;
	//					App->resources->resources.erase(item);
	//					break;
	//				}
	//			}
	//		}
	//	}

	//	ResourceScene* scene = new ResourceScene();
	//	scene->SetAssetsPath(path.data());
	//	scene->CreateMetaData();
	//	App->objects->CreateEmptyScene(scene);
	//	App->objects->SaveScene(scene);

	//	// last of all, refresh nodes because I have no idea if the user has created folders or moved things in the explorer. Users are bad people creating folders without using the alien engine explorer :(
	//	App->ui->panel_project->RefreshAllNodes();
	//}
	//else {
	//	SetCurrentDirectoryA(curr_dir);
	//}
}

void PanelSceneSelector::MenuSaveCurrentScene()
{
	ImGui::OpenPopup("Scene might have been modified");
	ImGui::SetNextWindowSize({ 240,140 });
	if (ImGui::BeginPopupModal("Scene might have been modified", &menu_save_current, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
	{
		ImGui::Spacing();

		ImGui::Text("Do you want to save changes in:");

		ImGui::Spacing();

		if (!App->objects->current_scenes.empty()) {
			if (App->objects->current_scenes.size() == 1) {
				ImGui::Text(App->file_system->GetBaseFileNameWithExtension(App->objects->current_scenes[0]->GetAssetsPath()).data());
			}
			else {
				ImGui::Text("Multiples scenes opened");
			}
		}
		else {
			ImGui::Text("Untitled*");
		}

		ImGui::Spacing();

		ImGui::Text("Your changes will be lost if you\ndon't save them.");

		ImGui::Spacing();
		ImGui::Spacing();

		if (ImGui::Button("Save", { 65,20 })) {
			OrganizeSave(SceneSelectorState::SAVE_SCENE);
			menu_save_current = false;
		}
		
		ImGui::SameLine();

		if (ImGui::Button("Don't save", { 80,20 })) {
			menu_save_current = false;
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel", { 65,20 })) {
			scene_to_load.clear();
			menu_save_current = false;
			load = false;
			create_new = false;
		}

		ImGui::EndPopup();
	}
	else {
		menu_save_current = false;
		load = false;
		create_new = false;
		scene_to_load.clear();
	}

	if (!menu_save_current) {
		if (load) {
			load = false;
			LoadScene();
		}
		else if (create_new) {
			create_new = false;
			CreateNewScene();
		}
		else if (!scene_to_load.empty()) {
			App->objects->LoadScene(App->file_system->GetBaseFileName(scene_to_load.data()).data());
			scene_to_load.clear();
		}
	}
}
