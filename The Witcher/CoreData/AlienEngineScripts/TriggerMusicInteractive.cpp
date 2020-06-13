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
		if(is_lvl1)
			emitter->SetState("Interactive_Music_Lvl1", GetNameByEnum(interactive).c_str());
		else
			emitter->SetState("Music_Lvl2_and_wagons", GetNameByEnum(interactive).c_str());

		m_controller->last_music = GetNameByEnum(interactive).c_str();
		m_controller->has_changed = true;
	}
}

std::string TriggerMusicInteractive::GetNameByEnum(Music mat)
{
	if (is_lvl1)
	{
		std::string name;
		switch (mat)
		{
		case Music::COMBAT:
			return name = "Combat";
			break;
		case Music::AFTER_BOSS:
			return name = "After_Boss";
			break;
		case Music::BOSS:
			return name = "Boss";
			break;
		case Music::CAVE:
			return name = "Cave";
			break;
		case Music::QUIET:
			return name = "Quiet";
			break;
		case Music::DROWNERS:
			return name = "Drowners";
			break;
		case Music::FORTRESS:
			return name = "Fortress";
			break;
		case Music::GHOUL_NESTS:
			return name = "Ghoul_Nests";
			break;
		case Music::MIDDLE_AREA:
			return name = "Middle_Area";
			break;
		case Music::PRE_BOSS:
			return name = "Pre_Boss";
			break;
		case Music::RAVINE:
			return name = "Ravine";
			break;
		case Music::SLOPE_TO_CAVE:
			return name = "Slope_To_Cave";
			break;
		case Music::MINE:
			return name = "Mine";
			break;
		}
	}
	else
	{
		std::string name;
		switch (mat)
		{
		case Music::BRIDGE:
			return name = "Bridge";
			break;
		case Music::COMBAT:
			return name = "Combat";
			break;
		case Music::DA_BIG_TRUNK:
			return name = "Da_Big_Trunk";
			break;
		case Music::ENTRY_ZONE:
			return name = "Entry_zone";
			break;
		case Music::GHOUL_NESTS:
			return name = "Ghoul_Nests";
			break;
		case Music::HALL:
			return name = "Hall";
			break;
		case Music::MINECARTS_CAVE:
			return name = "Minecarts_cave";
			break;
		case Music::QUIET:
			return name = "Quiet";
			break;
		}
	}
	

}
