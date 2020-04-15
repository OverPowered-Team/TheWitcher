#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include <queue>

class PlayerController;

class Notification : public Alien
{
public:

	bool is_geralt = false;
	const char* relic_name = "";
	const char* description = "";

};

class ALIEN_ENGINE_API Relic_Notification : public Alien {

public:

	Relic_Notification();
	virtual ~Relic_Notification();
	
	void Start();
	void Update();

	void TriggerRelic(PlayerController* player, std::string relic_name);
	void ShowRelic(Notification* notification);
	void StopRelic();

public:
	float time_to_show = 5.0f;

private:
	float time = 0.0f;

	GameObject* relic_notification = nullptr;
	GameObject* geralt_portrait = nullptr;
	GameObject* yennefer_portrait = nullptr;
	ComponentText* relic_title = nullptr;

	Notification* active = nullptr;
	std::queue<Notification*> notifications;
};

ALIEN_FACTORY Relic_Notification* CreateRelic_Notification() {
	Relic_Notification* alien = new Relic_Notification();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->time_to_show);

	return alien;
} 
