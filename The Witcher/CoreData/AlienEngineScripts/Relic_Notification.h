#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include <queue>
#include "PlayerController.h"

class Notification
{
public:

	PlayerController::PlayerType type;
	const char* relic_name = nullptr;
	const char* description = nullptr;
	std::string attack;
};

class ALIEN_ENGINE_API Relic_Notification : public Alien {

public:

	Relic_Notification();
	virtual ~Relic_Notification();
	
	void Start();
	void Update();

	void TriggerRelic(PlayerController* player, const std::string& relic_name, const std::string& description, const std::string& attack_combo);
	void ShowRelic(Notification* notification);
	void StopRelic();

public:
	float time_to_show = 5.0f;

private:
	float time = 0.0f;

	GameObject* relic_notification = nullptr;
	GameObject* geralt_portrait = nullptr;
	GameObject* yennefer_portrait = nullptr;
	GameObject* combo = nullptr;
	ComponentText* relic_title = nullptr;
	ComponentText* description = nullptr;

	Notification* active = nullptr;
	std::queue<Notification*> notifications;
	std::vector<GameObject*> L_combo_images;
	std::vector<GameObject*> H_combo_images;
};

ALIEN_FACTORY Relic_Notification* CreateRelic_Notification() {
	Relic_Notification* alien = new Relic_Notification();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(alien->time_to_show);

	return alien;
} 
