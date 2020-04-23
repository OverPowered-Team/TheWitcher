#include "TriggerEnvironment.h"
#include "PlayerController.h"
#include "CameraMovement.h"

TriggerEnvironment::TriggerEnvironment() : Alien()
{
}

TriggerEnvironment::~TriggerEnvironment()
{
}

void TriggerEnvironment::Start()
{
	
	camera = Camera::GetCurrentCamera()->game_object_attached;
	cam_script = camera->GetComponent<CameraMovement>();
	emitter = game_object->GetComponent<ComponentAudioEmitter>();
	emitter->SetState("Env_Lvl1", "Quiet");
}

void TriggerEnvironment::Update()
{
}

void TriggerEnvironment::OnTriggerEnter(ComponentCollider* collider)
{
	Component* c = (Component*)collider;
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0)
	{
		if (p1 == nullptr)
		{
			p1 = c->game_object_attached;
			player_counter++;
			LOG("ENTER p1");
		}
		else if (p2 == nullptr)
		{
			if (c->game_object_attached != p1)
			{
				p2 = c->game_object_attached;
				player_counter++;
				LOG("ENTER p2");
			}
		}
		if (emitter != nullptr && player_counter == 2) {
			emitter->SetState("Env_Lvl1", GetNameByEnum(environment).c_str());

			LOG("ENTER");
		}
	}
}

void TriggerEnvironment::OnTriggerExit(ComponentCollider* collider)
{
	Component* c = (Component*)collider;
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0) {
		if (c->game_object_attached == p1)
		{
			p1 = nullptr;
			player_counter--;
			LOG("EXIT p1");
		}
		else if (c->game_object_attached == p2)
		{
			p2 = nullptr;
			player_counter--;
			LOG("EXIT p2");
		}
		if (emitter != nullptr && player_counter == 0) {
			emitter->SetState("Env_Lvl1", "Quiet");
			LOG("EXIT");
		}
	}
}

std::string TriggerEnvironment::GetNameByEnum(Environment mat)
{
	std::string name;
	switch (mat)
	{
	case Environment::WIND:
		return name = "Wind";
		break;
	case Environment::FOREST:
		return name = "Forest";
		break;
	case Environment::BIRDS_WIND:
		return name = "Birds_Wind";
		break;
	}
}
