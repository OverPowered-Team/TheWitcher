#include "CameraEnterExit.h"
#include "PlayerController.h"
#include "TriggerCamera.h"

CameraEnterExit::CameraEnterExit() : Alien()
{
}

CameraEnterExit::~CameraEnterExit()
{
}

void CameraEnterExit::Start()
{
	script_parent = (TriggerCamera*)game_object->GetComponentScriptInParent("TriggerCamera");
}

void CameraEnterExit::OnTriggerEnter(ComponentCollider* collider)
{
	PlayerController* player = (PlayerController*)collider->game_object_attached->GetComponentScriptInParent("PlayerController");
	if (player != nullptr) {
		script_parent->RegisterMovement(player->controller_index - 1, collider_position);//INFO: Controller index starts by 1
	}
}
