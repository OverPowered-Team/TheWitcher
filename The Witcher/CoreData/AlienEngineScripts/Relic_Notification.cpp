#include "Relic_Notification.h"
#include "PlayerController.h"

Relic_Notification::Relic_Notification() : Alien()
{
}

Relic_Notification::~Relic_Notification()
{
}

void Relic_Notification::Start()
{
	relic_notification = GameObject::FindWithName("Relics_Notification");
	geralt_portrait = game_object->GetChild("Relics_Notification")->GetChild("Portrait_Relics")->GetChild("Geralt_Relics_Portrait");
	yennefer_portrait = game_object->GetChild("Relics_Notification")->GetChild("Portrait_Relics")->GetChild("Yennefer_Relics_Portrait");
	relic_title = (ComponentText*)game_object->GetChild("Relics_Notification")->GetChild("Relic_Name")->GetComponent(ComponentType::UI_TEXT);
}

void Relic_Notification::Update()
{
	if (!notifications.empty() && active == nullptr)
	{
		ShowRelic(notifications.front());
	}

	if ((time + time_to_show < Time::GetTimeSinceStart()) && active)
	{
		StopRelic();
	}
}

void Relic_Notification::TriggerRelic(PlayerController* player, std::string relic_name, std::string description)
{
	Notification* new_relic = new Notification();
	new_relic->type = player->player_data.player_type;
	new_relic->relic_name = "No Name";
	new_relic->description = "Description";
	//new_relic->relic_name = relic_name.c_str();
	//new_relic->description = description.c_str();
	notifications.push(new_relic);
}

void Relic_Notification::ShowRelic(Notification* notification)
{
	relic_notification->SetEnable(true);

	if (notification->type == PlayerController::PlayerType::GERALT)
	{
		geralt_portrait->SetEnable(true);
		yennefer_portrait->SetEnable(false);
	}
	else
	{
		geralt_portrait->SetEnable(false);
		yennefer_portrait->SetEnable(true);
	}

	relic_title->SetText(notification->relic_name);
	relic_title->SetText(notification->description);
	active = notification;
	notifications.pop();
	time = Time::GetTimeSinceStart();
}

void Relic_Notification::StopRelic()
{
	relic_notification->SetEnable(false);
	delete active;
	active = nullptr;
}
