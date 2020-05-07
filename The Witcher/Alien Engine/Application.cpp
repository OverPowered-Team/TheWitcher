#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleUI.h"
#include "AnimTween.h"
#include "ModulePhysX.h"
#include "ModuleObjects.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "ModuleAudio.h"
#include "ShortCutManager.h"
#include "Parson/parson.h"
#include "Time.h"
#include "Skybox.h"
#include "ModuleFadeToBlack.h"
#include "ResourceShader.h"
#include "mmgr/mmgr.h"
#include "Optick/include/optick.h"
#include "Event.h"

Application::Application()
{
	OPTICK_EVENT();
	window = new ModuleWindow();
	input = new ModuleInput();
	renderer3D = new ModuleRenderer3D();
#ifndef GAME_VERSION
	camera = new ModuleCamera3D();
	ui = new ModuleUI();
#endif
	importer = new ModuleImporter();
	tween = new AnimTween();
	fade_to_black = new FadeToBlack();
	objects = new ModuleObjects();
	physx = new ModulePhysX();
	file_system = new ModuleFileSystem();
	resources = new ModuleResources();
	audio = new ModuleAudio();

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	LoadDll();

	// Main Modules
	AddModule(window);
	AddModule(input);
	AddModule(file_system);
	AddModule(resources);
	AddModule(importer);
	AddModule(audio);
	// Scenes
	AddModule(physx);
	AddModule(fade_to_black);
	AddModule(objects);
	AddModule(tween);
#ifndef GAME_VERSION
	AddModule(camera);
	AddModule(ui);
#endif
	// Renderer last!
	AddModule(renderer3D);

	window->segment_width = (WINDOW_ICON_WIDTH - BAR_BEGIN_POS * 2) / (list_modules.size() * 2);
}

void Application::LoadDll()
{
	OPTICK_EVENT();
	static char curr_dir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, curr_dir);
	dll = std::string(curr_dir + std::string("/") + DLL_WORKING_PATH);
	file_system->NormalizePath(dll);

#ifndef GAME_VERSION
	if (file_system->Exists(DLL_CREATION_PATH)) {
		remove(DLL_WORKING_PATH);
		while (MoveFileA(DLL_CREATION_PATH, DLL_WORKING_PATH) == FALSE) {}
	}
#endif

	scripts_dll = LoadLibrary(dll.data());
}

Application::~Application()
{
	std::list<Module*>::reverse_iterator item = list_modules.rbegin();

	while(item != list_modules.rend())
	{
		delete(*item);
		++item;
	}

	list_modules.clear();

	std::list<JSONfilepack*>::iterator json_item = json_files.begin();
	for (; json_item != json_files.end(); ++json_item) {
		if (*json_item != nullptr) {
			delete* json_item;
			*json_item = nullptr;
		}
	}
	json_files.clear();

	if (shortcut_manager != nullptr) {
		delete shortcut_manager;
	}

	FreeLibrary(scripts_dll);
}

void Application::LoadDefaultConfig()
{
	if (FileExists("Configuration/CustomConfiguration.json")) {
		remove("Configuration/CustomConfiguration.json");
		json_files.remove(config);
		delete config;
		config = nullptr;
	}
	config = LoadJSONFile("Configuration/DefaultConfiguration.json");
	if (config != nullptr) {
		LoadConfig();
	}
}

void Application::SaveCustomConfig()
{
	remove("Configuration/CustomConfiguration.json");
	config = CreateJSONFile("Configuration/CustomConfiguration.json");
	if (config != nullptr)
		SaveConfig();
}

JSONfilepack* Application::GetJSONLayout()
{
	return layout;
}

bool Application::LoadConfig()
{
	bool ret = true;

	fps_cap = config->GetBoolean("Configuration.Application.CapFPS");
	fps_limit = config->GetNumber("Configuration.Application.LimitFPS");

	if (fps_limit > 0)
	{
		framerate_cap = 1000 / fps_limit;
	}

	std::list<Module*>::iterator item = list_modules.begin();

	while (item != list_modules.end())
	{
		(*item)->LoadConfig(config);
		++item;
	}

	return true;
}

bool Application::SaveConfig()
{
	bool ret = true;

	config->StartSave();

	config->SetBoolean("Configuration.Application.CapFPS", fps_cap);
	config->SetNumber("Configuration.Application.LimitFPS", fps_limit);

	std::list<Module*>::iterator item = list_modules.begin();

	while (item != list_modules.end())
	{
		(*item)->SaveConfig(config);
		++item;
	}

	config->FinishSave();

	return true;
}

inline bool Application::FileExists(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

bool Application::Init()
{
	OPTICK_EVENT();
	bool ret = true;

	if (FileExists("Configuration/CustomConfiguration.json")) { // look if it has custom settings
		config = LoadJSONFile("Configuration/CustomConfiguration.json");
	}
	else { // load default settings if no custom
		config = LoadJSONFile("Configuration/DefaultConfiguration.json");
	}

	if (config != nullptr)
		LoadConfig();

	layout = LoadJSONFile("Configuration/LayoutsInfo.json");

#ifndef GAME_VERSION
	shortcut_manager = new ShortCutManager();
#endif
	// Call Init() in all modules
	std::list<Module*>::iterator item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = (*item)->Init();
		window->IncreaseBar();
		++item;
	}

	// After all Init calls we call Start() in all modules
	LOG_ENGINE("Application Start --------------");
	item = list_modules.begin();

	Time::Start();
	while(item != list_modules.end() && ret == true)
	{
		ret = (*item)->Start();
		window->IncreaseBar();
		++item;
	}

	ret = window->CreateCoreWindow();

#ifdef GAME_VERSION
	renderer3D->OnResize(window->width, window->height);
#endif

	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	OPTICK_EVENT();
	frame_count++;
	last_sec_frame_count++;
	dt = frame_time.ReadSec();
	if (Time::is_paused) {
		Time::engine_dt = 0;
		Time::SetDT(0);
		dt = 0;
	}
	else {
		Time::engine_dt = dt;
		if (Time::IsPlaying()) {
			Time::SetDT(dt);
		}
		else {
			Time::SetDT(0);
		}
	}
	frame_time.Start();
	ptimer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	OPTICK_EVENT();
	if (last_sec_frame_time.Read() > 1000)
	{
		prev_last_sec_frame_count = last_sec_frame_count;
		last_sec_frame_count = 0;
		last_sec_frame_time.Start();
	}

	float avg_fps = float(frame_count) / startup_time.ReadSec();
	float seconds_since_startup = startup_time.ReadSec();
	unsigned __int32 last_frame_ms = frame_time.Read();
	unsigned __int32 frames_on_last_update = prev_last_sec_frame_count;

	if (framerate_cap > 0 && last_frame_ms < framerate_cap && fps_cap)
	{
		j1PerfTimer time;
		float delaytimestart = time.ReadMs();
		SDL_Delay(framerate_cap - last_frame_ms);
		float delaytimefinish = time.ReadMs();
	}
	Time::Update();
#ifndef GAME_VERSION
	ui->FramerateRegister((float)prev_last_sec_frame_count, (float)(framerate_cap));
#endif
}

JSONfilepack* Application::LoadJSONFile(const std::string& path)
{
	OPTICK_EVENT();

	JSON_Value* value = json_parse_file(path.data());
	JSON_Object* object = json_value_get_object(value);

	if (value == nullptr || object == nullptr)
	{
		LOG_ENGINE("Error loading %s", path);
		return nullptr;
	}
	else {
		json_files.push_back(new JSONfilepack(path.data(), object, value));
		return json_files.back();
	}
}

JSONfilepack* Application::CreateJSONFile(const std::string& path)
{
	OPTICK_EVENT();

	JSON_Value* value = json_value_init_object();
	JSON_Object* object = json_value_get_object(value);
	json_serialize_to_file_pretty(value, path.data());
	if (value == nullptr || object == nullptr) {
		LOG_ENGINE("Error creating JSON with path %s", path.data());
		return nullptr;
	}
	else {
		json_files.push_back(new JSONfilepack(path.data(), object, value));
		return json_files.back();
	}
}

void Application::DeleteJSONfile(JSONfilepack* json_pack)
{
	OPTICK_EVENT();

	std::list<JSONfilepack*>::iterator item = json_files.begin();
	for (; item != json_files.end(); ++item) {
		if (*item != nullptr && *item == json_pack) {
			delete* item;
			*item = nullptr;
			json_files.remove(*item);
			break;
		}
	}
}

void Application::UpdateLogFile(FILE* fp)
{
	OPTICK_EVENT();

	for (auto it_log = engine_string_logs.begin(); it_log != engine_string_logs.end(); it_log++)
	{
		fprintf(fp, std::string((*it_log).loged.at(0).second + '\n').data());
	}
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	OPTICK_EVENT();
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	

	PreUpdate(ret);

#ifndef GAME_VERSION
	shortcut_manager->UpdateShortCuts();
#endif
	OnUpdate(ret);

	PostUpdate(ret);
	if (quit)
		ret = UPDATE_STOP;
	FinishUpdate();

	return ret;
}

void Application::PreUpdate(update_status& ret)
{
	OPTICK_EVENT();
	auto item = list_modules.begin();
	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		if ((*item)->IsEnabled())
		{
			ret = (*item)->PreUpdate(dt);
		}
		//assert(ret == UPDATE_CONTINUE);
		++item;
	}
}

void Application::OnUpdate(update_status& ret)
{
	OPTICK_EVENT();
	auto item = list_modules.begin();
	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		if ((*item)->IsEnabled())
		{
			ret = (*item)->Update(dt);
		}
		//assert(ret == UPDATE_CONTINUE);
		++item;
	}
}

void Application::PostUpdate(update_status& ret)
{
	OPTICK_EVENT();
	auto item = list_modules.begin();
	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		if ((*item)->IsEnabled())
		{
			ret = (*item)->PostUpdate(dt);
		}
		//assert(ret == UPDATE_CONTINUE);
		++item;
	}
}

bool Application::CleanUp()
{
	OPTICK_EVENT();
	bool ret = true;

	std::list<Module*>::reverse_iterator item = list_modules.rbegin();
	Time::CleanUp();
	while(item != list_modules.rend() && ret == true)
	{
		ret = (*item)->CleanUp();
		if (!ret)
			LOG_ENGINE("Module %s failed to CleanUp", (*item)->name);
		++item;
	}
	return ret;
}


bool Application::StringCmp(const char* str1, const char* str2)
{
	OPTICK_EVENT();

	size_t size = strlen(str1);
	if (size != strlen(str2))
		return false;

	for (uint i = 0; i < size; ++i) {
		if (std::tolower(str1[i]) != std::tolower(str2[i]))
			return false;
	}
	return true;
}

void Application::QuitApp()
{
	quit = true;
}

bool Application::IsQuiting() const
{
	return quit;
}

void Application::OpenWebsite(const std::string& website)
{
	ShellExecuteA(NULL, "open", website.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void Application::CastEvent(EventType eventType)
{
	OPTICK_EVENT();

	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); ++item)
		(*item)->HandleEvent(eventType);
}

void Application::SendAlienEvent(void* object, AlienEventType type)
{
	OPTICK_EVENT();

	AlienEvent alien_event;
	alien_event.object = object;
	alien_event.type = type;

	App->objects->HandleAlienEvent(alien_event);
	App->resources->HandleAlienEvent(alien_event);
}


void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}