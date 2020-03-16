#pragma once

#include "Module.h"
#include "Globals.h"
#include "WwiseT.h"

#include <list>
#include <map>

class ComponentAudioEmitter;

struct Bank {
	u64 id = 0ULL;
	std::string name = "";
	std::map<u64, std::string> events;
	std::map<u64, std::string> audios;
	bool loaded = false;
};


class ModuleAudio : public Module
{
public:
	ModuleAudio(bool start_enabled = true);
	~ModuleAudio();
	bool Start();
	void LoadBanksInfo();
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();
	bool UnloadAllBanksFromWwise();
	void AddBank(Bank* bk);
	// Utils
	void Play();
	void Stop();
	void Pause() const;
	void Resume() const;
	void SetListener(WwiseT::AudioSource* new_listener);
	WwiseT::AudioSource* CreateSoundEmitter(const char * name);
	const std::vector<Bank*> GetBanks() const;
	const Bank* GetBankByName(const char* name) const;
	Bank* GetBankByID(const u64& id) const;
	const char* GetEventNameByID(const u64& id) const;

	void HandleEvent(EventType eventType) override;

private:
	std::vector <Bank*> banks;
	bool play_mode = false;

public:
	std::list<ComponentAudioEmitter*> emitters;
	//std::list<Bank*> used_banks; TODO: in case of low memory performance optimize only loading the banks we are going to load
	bool is_playing = false;
	WwiseT::AudioSource* listener = nullptr;
	WwiseT::AudioSource* default_listener = nullptr;
};
