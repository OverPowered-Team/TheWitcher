#include "Application.h"
#include "ModuleAudio.h"
#include "ComponentAudioEmitter.h"
#include "Event.h"
#include "mmgr/mmgr.h"

#include "Optick/include/optick.h"

ModuleAudio::ModuleAudio(bool start_enabled) : Module(start_enabled)
{
	name = "audio";
}

ModuleAudio::~ModuleAudio()
{}

bool ModuleAudio::Start()
{
	// Init wwise and audio banks
	bool ret = WwiseT::InitSoundEngine();
	
	default_listener = CreateSoundEmitter("Listener");
	SetListener(default_listener);

	for (auto i = banks.begin(); i != banks.end(); i++) {
		if (!(*i)->loaded) {
			WwiseT::LoadBank((std::to_string((*i)->id) + ".bnk").c_str());
			(*i)->loaded = true;
		}
	}
	return ret;
}

void ModuleAudio::LoadBanksInfo()
{
	auto j = App->LoadJSONFile("Assets/AudioBanks/SoundbanksInfo.json");
	auto bank_arr = j->GetArray("SoundBanksInfo.SoundBanks");
	bank_arr->GetFirstNode();
	for (uint i = 0; i < bank_arr->GetArraySize(); ++i, bank_arr->GetAnotherNode()) {
		if (strcmp(bank_arr->GetString("ShortName"), "Init") != 0) {
			Bank* b = new Bank();
			b->id = std::stoull(bank_arr->GetString("Id"));
			b->name = bank_arr->GetString("ShortName");
			auto events = bank_arr->GetArray("IncludedEvents");
			events->GetFirstNode();
			for (uint e = 0; e < events->GetArraySize(); ++e, events->GetAnotherNode()) {
				b->events[std::stoull(events->GetString("Id"))] = events->GetString("Name");
			}
			auto aud = bank_arr->GetArray("IncludedMemoryFiles");
			for (uint a = 0; a < aud->GetArraySize(); ++a) {
				b->audios[std::stoull(aud->GetString("Id"))] = aud->GetString("ShortName");
			}
			banks.push_back(b);
		}
	}
}

update_status ModuleAudio::Update(float dt)
{
	OPTICK_EVENT();

	return UPDATE_CONTINUE;
}

update_status ModuleAudio::PostUpdate(float dt)
{
	OPTICK_EVENT();
	if (listener != nullptr)
		WwiseT::ProcessAudio();
	return UPDATE_CONTINUE;
}

bool ModuleAudio::CleanUp()
{
	OPTICK_EVENT();
	for (auto i = emitters.begin(); i != emitters.end(); i++)
	{
		if((*i))
			delete* i;
	}		
	emitters.clear();

	WwiseT::StopAllEvents();

	UnloadAllBanksFromWwise();
	for (auto b = banks.begin(); b != banks.end(); b++)
	{
		if ((*b))
			delete* b;
	}	
	banks.clear();

	delete default_listener;
	default_listener = nullptr;

	return WwiseT::CloseSoundEngine();
}

bool ModuleAudio::UnloadAllBanksFromWwise()
{
	OPTICK_EVENT();
	for (auto it = banks.begin(); it != banks.end(); it++)
	{
		if ((*it))
		{
			if ((*it)->loaded) {
				WwiseT::UnLoadBank(std::to_string((*it)->id).c_str());
				(*it)->loaded = false;
			}
		}
	}

	return true;
}

void ModuleAudio::AddBank(Bank* bk)
{
	banks.push_back(bk);
}

WwiseT::AudioSource * ModuleAudio::CreateSoundEmitter(const char * name)
{
	return WwiseT::CreateAudSource(name);
}

const std::vector<Bank*> ModuleAudio::GetBanks() const
{
	return banks;
}

const Bank* ModuleAudio::GetBankByName(const char* name) const
{
	Bank* bk = nullptr;
	for (int i = 0; i < banks.size(); ++i)
	{
		if (App->StringCmp(name, App->audio->GetBanks()[i]->name.c_str()))
			bk = App->audio->GetBanks()[i];
	}
	return bk;
}

Bank* ModuleAudio::GetBankByID(const u64& id) const
{
	for (auto i = banks.begin(); i != banks.end(); ++i)
	{
		if ((*i)->id == id)
			return *i;
	}
	return nullptr;
}

const char* ModuleAudio::GetEventNameByID(const u64& id) const
{
	for (auto i = banks.begin(); i != banks.end(); ++i)
	{
		for (auto j = (*i)->events.begin(); j != (*i)->events.end(); ++j)
		{
			if ((*j).first == id)
				return (*j).second.c_str();
		}
	}
	return "";
}

void ModuleAudio::HandleEvent(EventType eventType)
{
	switch (eventType)
	{
	case EventType::ON_PAUSE:
		//Pause();
		break;
	case EventType::ON_STOP:
		Stop();
		break;
	}
}

void ModuleAudio::Play()
{
	OPTICK_EVENT();
	for (auto iterator = emitters.begin(); iterator != App->audio->emitters.end(); ++iterator)
	{
		(*iterator)->StartSound();
	}

	is_playing = true;
}

void ModuleAudio::Stop()
{
	WwiseT::StopAllEvents();
}

void ModuleAudio::Pause() const
{
	WwiseT::PauseAll();
}

void ModuleAudio::Resume() const
{
	WwiseT::ResumeAll();
}

void ModuleAudio::SetListener(WwiseT::AudioSource* new_listener)
{
	if (new_listener == nullptr)
		listener = default_listener;
	else
	listener = new_listener;

	WwiseT::SetDefaultListener(new_listener->GetID());
}
