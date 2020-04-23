#include "TriggerMusicInteractive.h"
#include "PlayerController.h"
#include "CameraMovement.h"
#include "MusicController.h"

TriggerMusicInteractive::TriggerMusicInteractive() : Alien()
{
}

TriggerMusicInteractive::~TriggerMusicInteractive()
{
}

void TriggerMusicInteractive::Start()
{	
	camera = Camera::GetCurrentCamera()->game_object_attached;
	cam_script = camera->GetComponent<CameraMovement>();
	emitter = camera->GetComponent<ComponentAudioEmitter>();
	m_controller = camera->GetComponent<MusicController>();
}

void TriggerMusicInteractive::Update()
{
}

void TriggerMusicInteractive::OnTriggerEnter(ComponentCollider* collider)
{
	Component* c = (Component*)collider;
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0)
	{
		emitter->SetState("Interactive_Music_Lvl1", GetNameByEnum(interactive).c_str());
		m_controller->last_music = GetNameByEnum(interactive).c_str();
		m_controller->has_changed = true;
	}
}

std::string TriggerMusicInteractive::GetNameByEnum(Music mat)
{
	std::string name;
	switch (mat)
	{
	case Music::COMBAT:
		return name = "Combat";
		break;
	case Music::NORMAL:
		return name = "Normal";
		break;
	case Music::MOUNTAIN:
		return name = "Mountain";
		break;
	case Music::LAKE:
		return name = "Lake";
		break;
	case Music::QUIET:
		return name = "Quiet";
		break;
	}
	

}
