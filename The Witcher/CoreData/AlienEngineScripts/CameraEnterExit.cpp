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

	tp1 = game_object->GetChild("TpPos1");
	tp2 = game_object->GetChild("TpPos2");

	tp1->GetComponent(ComponentType::MESH)->SetEnable(false);
	tp2->GetComponent(ComponentType::MESH)->SetEnable(false);
}

void CameraEnterExit::Update()
{
}

void CameraEnterExit::OnTriggerEnter(ComponentCollider* collider)
{
	if (script_parent != nullptr && script_parent->cam_script != nullptr) {
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
						LOG("%s enter first", (*it).first->GetName());
						break;
					case CameraMovement::PlayerState::EXIT_FIRST:
						players->at(collider->game_object_attached) = CameraMovement::PlayerState::EXIT_FIRST_MIDDLE;
						LOG("%s exit first middle", (*it).first->GetName());
						break;
					case CameraMovement::PlayerState::EXIT_LAST:
						players->at(collider->game_object_attached) = CameraMovement::PlayerState::ENTER_FIRST;
						LOG("%s enter first", (*it).first->GetName());
						break;
						/*case CameraMovement::PlayerState::ON_EXIT:
							break;
						case CameraMovement::PlayerState::ENTER_FIRST:
							break;
						case CameraMovement::PlayerState::ENTER_LAST:
							break;*/
					default:
						LOG("%s default %i", (*it).first->GetName(), (int)(*it).second);
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
						LOG("%s exit first", (*it).first->GetName());
						break;
					case CameraMovement::PlayerState::ENTER_FIRST:
						players->at(collider->game_object_attached) = CameraMovement::PlayerState::ENTER_FIRST_MIDDLE;
						LOG("%s exit last middle", (*it).first->GetName());
						break;
					case CameraMovement::PlayerState::EXIT_LAST:
						players->at(collider->game_object_attached) = CameraMovement::PlayerState::ENTER_FIRST;
						LOG("%s enter first", (*it).first->GetName());
						break;
						/*case CameraMovement::PlayerState::EXIT_FIRST:
							break;
						case CameraMovement::PlayerState::ON_EXIT:
							break;*/
					case CameraMovement::PlayerState::ENTER_LAST:
						players->at(collider->game_object_attached) = CameraMovement::PlayerState::EXIT_FIRST;
						LOG("%s exit first", (*it).first->GetName());
						break;
					default:
						LOG("%s default %i", (*it).first->GetName(), (int)(*it).second);
						break;
					}
				}
				break;
			}
			default:
				break;
			}
	}
	else {
		LOG("THERE IS NOT CAMERA MOVEMENT SCRIPT IN CURRENT CAMERA");
	}
}

void CameraEnterExit::OnTriggerExit(ComponentCollider* collider)
{
	if (script_parent != nullptr && script_parent->cam_script != nullptr) {
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
						LOG("%s enter first", (*it).first->GetName());
						break;
					case CameraMovement::PlayerState::EXIT_FIRST:
						players->at(collider->game_object_attached) = CameraMovement::PlayerState::ENTER_LAST;
						LOG("%s enter last", (*it).first->GetName());
						break;
					case CameraMovement::PlayerState::ENTER_FIRST:
						players->at(collider->game_object_attached) = CameraMovement::PlayerState::NONE;
						LOG("%s none", (*it).first->GetName());
						break;
					case CameraMovement::PlayerState::ENTER_FIRST_MIDDLE:
						players->at(collider->game_object_attached) = CameraMovement::PlayerState::EXIT_LAST;
						LOG("%s exit last", (*it).first->GetName());
						break;
					case CameraMovement::PlayerState::EXIT_FIRST_MIDDLE:
						players->at(collider->game_object_attached) = CameraMovement::PlayerState::EXIT_FIRST;
						LOG("%s exit first", (*it).first->GetName());
						break;
					case CameraMovement::PlayerState::ENTER_LAST: {
						players->at(collider->game_object_attached) = CameraMovement::PlayerState::ON_EXIT;
						LOG("%s ON EXIT", (*it).first->GetName());

						script_parent->InterChangeInfoWithCamera();
						script_parent->ManageTransition(false);

						GameObject* tp = tp1;
						float3 gobj_pos = collider->game_object_attached->transform->GetGlobalPosition();
						float dis1 = gobj_pos.Distance(tp1->transform->GetGlobalPosition());
						if (dis1 < gobj_pos.Distance(tp2->transform->GetGlobalPosition()))
							tp = tp2;
						for (auto i = players->begin(); i != players->end(); ++i) {
							(*i).second = CameraMovement::PlayerState::NONE;
							if ((*i).first != collider->game_object_attached) {
								(*i).first->transform->SetGlobalPosition(tp->transform->GetGlobalPosition());
							}
						}
						break;
					}
					case CameraMovement::PlayerState::ON_EXIT:
						break;
					case CameraMovement::PlayerState::EXIT_LAST:
						break;
					default:
						LOG("%s default %i", (*it).first->GetName(), (int)(*it).second);
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
						LOG("%s exit first", (*it).first->GetName());
						break;
						/*case CameraMovement::PlayerState::ENTER_FIRST:
							players->at(collider->game_object_attached) = CameraMovement::PlayerState::EXIT_LAST;
							LOG("%s exit last", (*it).first->GetName());
							break;*/
					case CameraMovement::PlayerState::EXIT_FIRST:
						players->at(collider->game_object_attached) = CameraMovement::PlayerState::NONE;
						LOG("%s none", (*it).first->GetName());
						break;
					case CameraMovement::PlayerState::ENTER_FIRST_MIDDLE:
						players->at(collider->game_object_attached) = CameraMovement::PlayerState::ENTER_FIRST;
						LOG("%s enter first", (*it).first->GetName());
						break;
					case CameraMovement::PlayerState::EXIT_FIRST_MIDDLE:
						players->at(collider->game_object_attached) = CameraMovement::PlayerState::ENTER_LAST;
						LOG("%s enter last", (*it).first->GetName());
						break;
					case CameraMovement::PlayerState::EXIT_LAST: {
						LOG("%s ON EXIT", (*it).first->GetName());

						players->at(collider->game_object_attached) = CameraMovement::PlayerState::ON_EXIT;
						script_parent->ManageTransition(true);

						GameObject* tp = tp1;
						float3 gobj_pos = collider->game_object_attached->transform->GetGlobalPosition();
						float dis1 = gobj_pos.Distance(tp1->transform->GetGlobalPosition());
						if (dis1 < gobj_pos.Distance(tp2->transform->GetGlobalPosition()))
							tp = tp2;
						for (auto i = players->begin(); i != players->end(); ++i) {
							(*i).second = CameraMovement::PlayerState::NONE;
							if ((*i).first != collider->game_object_attached) {
								(*i).first->transform->SetGlobalPosition(tp->transform->GetGlobalPosition());
							}
						}

						break;
					}
					case CameraMovement::PlayerState::ON_EXIT:
						break;
					case CameraMovement::PlayerState::ENTER_LAST:
						break;
					default:
						LOG("%s default %i", (*it).first->GetName(), (int)(*it).second);
						break;
					}
				}
				break;
			}
			default:
				break;
			}
	}
	else {
		LOG("THERE IS NOT CAMERA MOVEMENT SCRIPT IN CURRENT CAMERA");
	}
}
