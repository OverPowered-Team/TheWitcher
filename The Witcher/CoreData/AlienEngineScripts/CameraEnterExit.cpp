#include "CameraEnterExit.h"

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

void CameraEnterExit::Update()
{
}

void CameraEnterExit::OnTriggerEnter(ComponentCollider* collider)
{
	auto players = &script_parent->cam_script->players;
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0)
		switch (type)
		{
		case CameraEnterExit::CameraTriggerType::ENTER: {
			auto it = players->find(collider->game_object_attached);
			if (it != players->end()) {
				switch ((*it).second)
				{
				case CameraMovement::PlayerState::NONE:
					players->at(collider->game_object_attached) = CameraMovement::PlayerState::ENTER_FIRST;
					break;
				case CameraMovement::PlayerState::EXIT_FIRST:
					players->at(collider->game_object_attached) = CameraMovement::PlayerState::ENTER_LAST;
					break;
				case CameraMovement::PlayerState::EXIT_LAST:
					players->at(collider->game_object_attached) = CameraMovement::PlayerState::ENTER_FIRST;
					break;
					/*case CameraMovement::PlayerState::ON_EXIT:
						break;
					case CameraMovement::PlayerState::ENTER_FIRST:
						break;
					case CameraMovement::PlayerState::ENTER_LAST:
						break;*/
				default:
					break;
				}
			}
			break;
		}
		case CameraEnterExit::CameraTriggerType::EXIT: {
			auto it = players->find(collider->game_object_attached);
			if (it != players->end()) {
				switch ((*it).second)
				{
				case CameraMovement::PlayerState::NONE:
					players->at(collider->game_object_attached) = CameraMovement::PlayerState::EXIT_FIRST;
					break;
				case CameraMovement::PlayerState::ENTER_FIRST:
					players->at(collider->game_object_attached) = CameraMovement::PlayerState::EXIT_LAST;
					break;
				case CameraMovement::PlayerState::EXIT_LAST:
					players->at(collider->game_object_attached) = CameraMovement::PlayerState::ENTER_FIRST;
					break;
					/*case CameraMovement::PlayerState::EXIT_FIRST:
						break;
					case CameraMovement::PlayerState::ON_EXIT:
						break;
					case CameraMovement::PlayerState::ENTER_LAST:
						break;*/
				default:
					break;
				}
			}
			break;
		}
		default:
			break;
		}
}

void CameraEnterExit::OnTriggerExit(ComponentCollider* collider)
{
	auto players = &script_parent->cam_script->players;
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0)
		switch (type)
		{
		case CameraEnterExit::CameraTriggerType::ENTER: {
			auto it = players->find(collider->game_object_attached);
			if (it != players->end()) {
				switch ((*it).second)
				{
				case CameraMovement::PlayerState::NONE:
					players->at(collider->game_object_attached) = CameraMovement::PlayerState::ENTER_FIRST;
					break;
				case CameraMovement::PlayerState::EXIT_FIRST:
					players->at(collider->game_object_attached) = CameraMovement::PlayerState::ENTER_LAST;
					break;
				case CameraMovement::PlayerState::ON_EXIT:
					break;
				case CameraMovement::PlayerState::ENTER_FIRST:
					players->at(collider->game_object_attached) = CameraMovement::PlayerState::NONE;
					break;
				case CameraMovement::PlayerState::ENTER_LAST:
					players->at(collider->game_object_attached) = CameraMovement::PlayerState::ON_EXIT;
					break;
				case CameraMovement::PlayerState::EXIT_LAST:
					break;
				default:
					break;
				}
			}
			break;
		}
		case CameraEnterExit::CameraTriggerType::EXIT: {
			auto it = players->find(collider->game_object_attached);
			if (it != players->end()) {
				switch ((*it).second)
				{
				case CameraMovement::PlayerState::NONE:
					players->at(collider->game_object_attached) = CameraMovement::PlayerState::EXIT_FIRST;
					break;
				case CameraMovement::PlayerState::ENTER_FIRST:
					players->at(collider->game_object_attached) = CameraMovement::PlayerState::EXIT_LAST;
					break;
				case CameraMovement::PlayerState::EXIT_FIRST:
					players->at(collider->game_object_attached) = CameraMovement::PlayerState::NONE;
					break;
				case CameraMovement::PlayerState::EXIT_LAST:
					players->at(collider->game_object_attached) = CameraMovement::PlayerState::ON_EXIT;
					break;
				case CameraMovement::PlayerState::ON_EXIT:
					break;
				case CameraMovement::PlayerState::ENTER_LAST:
					break;
				default:
					break;
				}
			}
			break;
		}
		default:
			break;
		}
}
