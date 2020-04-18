#pragma once

#include "imgui/imgui.h"
#include <regex>
#include "ModulePhysics.h"
#include "j1PerfTimer.h"
#include "Timer.h"

#include <list>
#include <string>
#include <vector>

enum class EventType; 
enum class AlienEventType;


class Module;
class ModuleWindow;
class ModuleInput;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModuleUI;
class ModuleImporter;
class ShortCutManager;
class ModuleObjects;
class ModuleFileSystem;
class ModuleResources;
class ModuleAudio;
class AnimTween;
class FadeToBlack;
class JSONfilepack;

struct LogInfo {
	LogInfo(const int& line, const char* file, const char* loged) {
		this->line = line;
		strcpy(this->file, file);
		this->loged.push_back({ 1,std::string(loged) });
		instances = 1;
	}
	int line = 0;
	bool opened = false;
	int instances = 0;
	char file[MAX_PATH];
	std::vector<std::pair<int, std::string>> loged;
};

class Application
{
public:
	ModuleWindow*		window = nullptr;
	ModuleInput*		input = nullptr;
	ModuleRenderer3D*	renderer3D = nullptr;
	ModuleCamera3D*		camera = nullptr;
	ModuleUI*			ui = nullptr;
	ModuleImporter*		importer = nullptr;
	ShortCutManager*	shortcut_manager = nullptr;
	ModuleObjects*		objects = nullptr;
	ModuleFileSystem*	file_system = nullptr;
	ModuleResources*	resources = nullptr;
	ModuleAudio*		audio = nullptr;
	ModulePhysics* 		physics = nullptr;
	AnimTween*			tween = nullptr;
	FadeToBlack*		fade_to_black = nullptr;

	bool fps_cap = true;
	uint16_t framerate_cap;
	int fps_limit = 30;
	std::vector<LogInfo> engine_string_logs;
	std::vector<LogInfo> game_string_logs;
	ImGuiTextBuffer all_engine_logs;
	ImGuiTextBuffer all_game_logs;
	HINSTANCE scripts_dll = nullptr;
	std::string dll;

private:
	JSONfilepack* config = nullptr;
	JSONfilepack* layout = nullptr;
	
	std::list<JSONfilepack*> json_files;
	std::list<Module*> list_modules;
	bool quit = false;

	j1PerfTimer			ptimer;
	unsigned __int64	frame_count = 0;
	Timer				startup_time;
	Timer				frame_time;
	Timer				last_sec_frame_time;
	unsigned __int32	last_sec_frame_count = 0;
	unsigned __int32	prev_last_sec_frame_count = 0;
	float				dt;

public:

	Application();
	void LoadDll();
	~Application();

	void LoadDefaultConfig();
	void SaveCustomConfig();

	JSONfilepack* GetJSONLayout();

	bool FileExists(const std::string& name);

	bool Init();
	update_status Update();
	bool CleanUp();

	bool StringCmp(const char* str1, const char* str2);

	void QuitApp();
	bool IsQuiting() const;

	void OpenWebsite(const std::string& website);
	void CastEvent(EventType eventType);
	void SendAlienEvent(void * object, AlienEventType type);

	JSONfilepack* LoadJSONFile(const std::string& path);
	JSONfilepack* CreateJSONFile(const std::string& path);
	void DeleteJSONfile(JSONfilepack* json_pack);

	void UpdateLogFile(FILE* fp);

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

	void PreUpdate(update_status&ret);
	void OnUpdate(update_status& ret);
	void PostUpdate(update_status& ret);

	bool LoadConfig();
	bool SaveConfig();
};

extern Application* App;
