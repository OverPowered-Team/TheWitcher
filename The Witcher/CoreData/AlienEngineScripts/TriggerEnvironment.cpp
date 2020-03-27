#include "TriggerEnvironment.h"
#include "PlayerController.h"
#include "CameraMovement.h"
#include "PlayerTest.h"

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
}

void TriggerEnvironment::Update()
{
}

void TriggerEnvironment::OnTriggerEnter(ComponentCollider* collider)
{
	if (emitter != nullptr) {
		emitter->SetSwitchState("Env_Lvl1", GetNameByEnum(environment).c_str());
		emitter->StartSound();
		LOG("ENTER");
	}
}

void TriggerEnvironment::OnTriggerExit(ComponentCollider* collider)
{
	if (emitter != nullptr) {
		emitter->SetSwitchState("Env_Lvl1", "Quiet");
		LOG("EXIT");
		emitter->StartSound();
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
