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
	if (current_notification && (notification_time + 5 < Time::GetGameTime()))
	{
		game_object->GetChild("Relics_Notification")->SetEnable(false);
		current_notification = false;
	}
}

void Relic_Notification::TriggerRelic(PlayerController* player, std::string relic_name)
{
	relic_notification->SetEnable(true);
	if (player->player_data.player_type == PlayerController::PlayerType::GERALT)
	{
		geralt_portrait->SetEnable(true);
		yennefer_portrait->SetEnable(false);
	}
	else
	{
		geralt_portrait->SetEnable(false);
		yennefer_portrait->SetEnable(true);
	}
	relic_title->SetText(relic_name.c_str());
	current_notification = true;
	notification_time = Time::GetGameTime();
	LOG("Trigger");
}
