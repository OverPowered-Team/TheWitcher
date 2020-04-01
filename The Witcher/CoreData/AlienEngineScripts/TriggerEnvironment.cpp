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
	cam_script = (CameraMovement*)camera->GetComponentScript("CameraMovement");
	emitter = (ComponentAudioEmitter*)this->game_object->GetComponent(ComponentType::A_EMITTER);
	timer = Time::GetGameTime();
	emitter->SetState("Env_Lvl1", "Quiet");
}

void TriggerEnvironment::Update()
{
}

void TriggerEnvironment::OnTriggerEnter(ComponentCollider* collider)
{
	Component* c = (Component*)collider;
	if (Time::GetGameTime() - timer >= 3.f)
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
	if (Time::GetGameTime() - timer >= 3.f)
	{
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
	}
}
