#pragma once

#include "Panel.h"

#include "FileNode.h"

class PanelProject : public Panel {
	friend class PanelAnimator;
public:

	PanelProject(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat = SDL_SCANCODE_UNKNOWN, const SDL_Scancode& key3_repeat_extra = SDL_SCANCODE_UNKNOWN);
	virtual ~PanelProject();
	void PanelLogic();

	bool SelectFile(const char* path, FileNode* node);
	FileNode* GetSelectedFile();
	void RefreshAllNodes();

	void GetUniqueFileName(std::string& asset_name, const std::string& asset_node);

public:

	FileNode* current_active_folder = nullptr;
	Resource* selected_resource = nullptr;

private:

	void PrintDirectoryNodes(FileNode* node);
	void SeeFiles();
	void DeleteSelectedAssetPopUp();
	void RightClickInFileOrFolder(const uint& i, bool& pop_up_item);
	void PrintNodeNameUnderIcon(const uint& i);
	void RightClickToWindow(bool pop_up_item);
	bool MoveToFolder(FileNode* node, bool inside);
	void DeleteNodes(FileNode* node);

	int colum_width[2];

	FileNode* assets = nullptr;

	FileNode* current_active_file = nullptr;
	FileNode go_back_folder;

	bool change_folder = false;
	bool to_delete_menu = false;
};
