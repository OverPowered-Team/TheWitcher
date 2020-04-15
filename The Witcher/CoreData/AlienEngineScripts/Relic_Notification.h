#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class PlayerController;

class ALIEN_ENGINE_API Relic_Notification : public Alien {

public:

	Relic_Notification();
	virtual ~Relic_Notification();
	
	void Start();
	void Update();

	void TriggerRelic(PlayerController* player, std::string relic_name);

private:
	float notification_time = 0.0f;
	bool current_notification = false;

	GameObject* relic_notification = nullptr;
	GameObject* geralt_portrait = nullptr;
	GameObject* yennefer_portrait = nullptr;
	ComponentText* relic_title = nullptr;
};

ALIEN_FACTORY Relic_Notification* CreateRelic_Notification() {
	Relic_Notification* alien = new Relic_Notification();
	// To show in inspector here

	return alien;
} 
