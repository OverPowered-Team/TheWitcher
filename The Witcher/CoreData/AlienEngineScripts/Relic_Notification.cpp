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
	if (relic_notification)
	{
		relic_notification->SetEnable(false);
		geralt_portrait = relic_notification->GetChild("Portrait_Relics")->GetChild("Geralt_Relics_Portrait");
		yennefer_portrait = relic_notification->GetChild("Portrait_Relics")->GetChild("Yennefer_Relics_Portrait");
		relic_title = relic_notification->GetChild("Relic_Name")->GetComponent<ComponentText>();
		description = relic_notification->GetChild("Relic_Description")->GetComponent<ComponentText>();
		combo = relic_notification->GetChild("Combo_Container");
		RB = combo->GetChild("RB");
		RB->SetEnable(false);
	}

	L_combo_images.resize(5);
	H_combo_images.resize(5);

	for (uint i = 0; i < 5 ; ++i)
	{
		L_combo_images[i] = combo->GetChild("L_Buttons")->GetChild(i);
		H_combo_images[i] = combo->GetChild("H_Buttons")->GetChild(i);
		L_combo_images[i]->SetEnable(false);
		H_combo_images[i]->SetEnable(false);
	}

	relic_images.push_back(relic_notification->GetChild("Relics")->GetChild("Fire"));
	relic_images.push_back(relic_notification->GetChild("Relics")->GetChild("Poison"));
	relic_images.push_back(relic_notification->GetChild("Relics")->GetChild("Ice"));
	relic_images.push_back(relic_notification->GetChild("Relics")->GetChild("Earth"));
	relic_images.push_back(relic_notification->GetChild("Relics")->GetChild("Lighting"));

	auto iter = relic_images.begin();
	for (; iter != relic_images.end(); ++iter)
	{
		(*iter)->SetEnable(false);
	}
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

void Relic_Notification::TriggerRelic(PlayerController* player, const std::string& relic_name, const std::string& description, const std::string& attack_combo, const std::string& element)
{
	Notification* new_relic = new Notification();
	new_relic->type = player->player_data.type;
	new_relic->attack = attack_combo;
	new_relic->relic_name = relic_name.c_str();
	new_relic->description = description.c_str();
	new_relic->attack_type = element;
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
	description->SetText(notification->description);
	if (notification->attack_type.size() != 0)
	{
		if (strcmp(notification->attack_type.c_str(), "Fire") == 0)
		{
			relic_images[0]->SetEnable(true);
		}
		else if (strcmp(notification->attack_type.c_str(), "Poison") == 0)
		{
			relic_images[1]->SetEnable(true);
		}
		else if (strcmp(notification->attack_type.c_str(), "Ice") == 0)
		{
			relic_images[2]->SetEnable(true);
		}
		else if (strcmp(notification->attack_type.c_str(), "Earth") == 0)
		{
			relic_images[3]->SetEnable(true);
		}
		else if (strcmp(notification->attack_type.c_str(), "Lightning") == 0)
		{
			relic_images[4]->SetEnable(true);
		}
	}

	if (!notification->attack.empty())
	{
		uint i = 0;
		uint s = 0;
		for (notification->attack.size() > 3 ? i = 0 : i = 1; s < notification->attack.size(); ++i)
		{
			if (notification->attack[s] == 'L')
			{
				L_combo_images[i]->SetEnable(true);
				H_combo_images[i]->SetEnable(false);
			}
			else if (notification->attack[s] == 'H')
			{
				L_combo_images[i]->SetEnable(false);
				H_combo_images[i]->SetEnable(true);
			}
			else if (notification->attack[s] == ' ')
			{
				L_combo_images[i]->SetEnable(false);
				H_combo_images[i]->SetEnable(false);
			}
			++s;

			if (s >= notification->attack.size())
			{
				GameObject* parent = nullptr;

				if (H_combo_images[i]->IsEnabled())
				{
					parent = H_combo_images[i];
				}
				else
				{
					parent = L_combo_images[i];
				}

				marker = GameObject::Instantiate(combo_marker, float3(0, parent->transform->GetLocalPosition().y - 0.125f, 0), false, parent);
			}
		}
	}
	else
	{
		RB->SetEnable(true);
	}

	active = notification;
	notifications.pop();
	time = Time::GetTimeSinceStart();
}

void Relic_Notification::StopRelic()
{
	relic_notification->SetEnable(false);
	for (uint i = 0; i < 5; ++i)
	{
		L_combo_images[i]->SetEnable(false);
		H_combo_images[i]->SetEnable(false);
	}

	if (RB->IsEnabled())
	{
		RB->SetEnable(false);
	}

	auto iter = relic_images.begin();
	for (; iter != relic_images.end(); ++iter)
	{
		(*iter)->SetEnable(false);
	}

	if (marker)
	{
		GameObject::Destroy(marker);
		marker = nullptr;
	}

	delete active;
	active = nullptr;
}
