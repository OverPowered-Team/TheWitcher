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
	emitter = game_object->GetComponent<ComponentAudioEmitter>();
	//I know this is ugly but right now we can't do it dynamically to show in inspector
	emitter->SetState("Env_Lvl1", "Quiet");
	env_elements.push_back(audio1);
	env_elements.push_back(audio2);
	env_elements.push_back(audio3);
	env_elements.push_back(audio4);
	env_elements.push_back(audio5);
	PrepareEnvironmentElements();

}

void TriggerEnvironment::Update()
{
	PlayEnvironment();
	MoveSpatial();
}

void TriggerEnvironment::CleanUp()
{
	env_elements.clear();
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
			emitter->SetState("Env_Lvl1", GetZoneByEnum(zone).c_str());
			PlayInstant();
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

void TriggerEnvironment::OnDrawGizmosSelected()
{
	for (auto iter = env_elements.begin(); iter != env_elements.end(); ++iter) {
		if (iter->spatial)
		{
			Color c;
			switch (iter - env_elements.begin())
			{
			case 0:
				c = Color::Blue();
				break;
			case 1:
				c = Color::Cyan();
				break;
			case 2:
				c = Color::Green();
				break;
			case 3:
				c = Color::Magenta();
				break;
			case 4:
				c = Color::Orange();
				break;
			}
			Gizmos::DrawWireSphere(iter->spatial_place.spatial_place->transform->GetGlobalPosition(), 0.2, c);
		}
	}
}

std::string TriggerEnvironment::GetAudioElementByEnum(AudioEffects eff)
{
	std::string name;
	switch (eff)
	{
	case AudioEffects::WIND:
		return name = "Play_Wind";
		break;
	case AudioEffects::FOREST:
		return name = "Play_Forest";
		break;
	case AudioEffects::BIRDS:
		return name = "Play_Bird";
		break;
	case AudioEffects::CROWS:
		return name = "Play_Crows";
		break;
	case AudioEffects::SQUIRRELS:
		return name = "Play_Squirrels";
		break;
	case AudioEffects::BATS:
		return name = "Play_Bats";
		break;
	case AudioEffects::CAVE:
		return name = "Play_Cave";
		break;
	case AudioEffects::GLOOMY:
		return name = "Play_Gloomy";
		break;
	case AudioEffects::INSECTS:
		return name = "Play_Insects";
		break;
	case AudioEffects::QUIET:
		return name = "Quiet";
		break;
	case AudioEffects::RIVER:
		return name = "Play_River";
		break;
	case AudioEffects::TREES_SQUEAKING:
		return name = "Play_Trees_Squeaking";
		break;
	case AudioEffects::TREE_FALL:
		return name = "Play_Tree_Fall";
		break;
	case AudioEffects::WOLF_HOWLING:
		return name = "Play_Wolf_Howling";
		break;
	}
}

std::string TriggerEnvironment::GetZoneByEnum(EnvironmentZone zone)
{
	std::string name;
	switch (zone)
	{
	case EnvironmentZone::QUIET:
		return name = "Quiet";
		break;
	case EnvironmentZone::AFTER_BOSS:
		return name = "After_Boss";
		break;
	case EnvironmentZone::CAVE:
		return name = "Cave";
		break;
	case EnvironmentZone::DROWNERS:
		return name = "Drowners";
		break;
	case EnvironmentZone::FORTRESS:
		return name = "Fortress";
		break;
	case EnvironmentZone::GHOUL_NESTS:
		return name = "Ghoul_Nests";
		break;
	case EnvironmentZone::MIDDLE_AREA:
		return name = "Middle_Area";
		break;
	case EnvironmentZone::PRE_BOSS:
		return name = "Pre_Boss";
		break;
	case EnvironmentZone::RAVINE:
		return name = "Ravine";
		break;
	case EnvironmentZone::SLOPE_TO_CAVE:
		return name = "Slope_To_Cave";
		break;
	}
}

void TriggerEnvironment::PrepareEnvironmentElements()
{
	for (int iter = 0; iter < env_elements.size(); ++iter)
	{		
		std::string spt = "Spatial";
		spt += std::to_string(iter);
		env_elements[iter].spatial_place.spatial_place = this->game_object->GetChild(spt.c_str());
		env_elements[iter].event_name = GetAudioElementByEnum(env_elements[iter].type);
		if (env_elements[iter].spatial_place.spatial_place != nullptr) {
			env_elements[iter].spatial_place.el_emitter = env_elements[iter].spatial_place.spatial_place->GetComponent<ComponentAudioEmitter>();
			if (env_elements[iter].spatial_movement)
			{
				std::string spt = "SpatialEnd";
				spt += std::to_string(iter);
				env_elements[iter].spatial_place.spatial_end = this->game_object->GetChild(spt.c_str());
				env_elements[iter].spatial_place.begin_pos = env_elements[iter].spatial_place.spatial_place->transform->GetGlobalPosition();
			}
		}
		if (!env_elements[iter].random && std::strcmp(env_elements[iter].event_name.c_str(), "QUIET") != 0 && env_elements[iter].spatial_place.el_emitter != nullptr && !env_elements[iter].instant) { //Play of the constant sounds ex: Wind
			env_elements[iter].spatial_place.el_emitter->StartSound(env_elements[iter].event_name.c_str());
		}
		else
		{
			env_elements[iter].timer_play = Time::GetGameTime();
			env_elements[iter].time_to_play = Random::GetRandomFloatBetweenTwo(1.f, env_elements[iter].max_time_between_plays);
		}
	}
	
}

void TriggerEnvironment::PlayEnvironment()
{
	for (auto iter = env_elements.begin(); iter != env_elements.end(); ++iter)
	{
		if (iter->random)
		{
			if (iter->spatial_movement && Time::GetGameTime() - iter->timer_play >= iter->time_to_play && iter->spatial_place.el_emitter != nullptr)
			{
				iter->spatial_place.el_emitter->StartSound(iter->event_name.c_str());
				iter->timer_play = Time::GetGameTime();
				iter->time_to_play = Random::GetRandomFloatBetweenTwo(iter->min_time_between_plays, iter->max_time_between_plays);
				iter->spatial_place.move = true;
			}

			if (!iter->spatial_movement && Time::GetGameTime() - iter->timer_play >= iter->time_to_play && iter->spatial_place.el_emitter != nullptr) //We play the random sounds given the seconds previously clamped between the selected values
			{
				iter->spatial_place.el_emitter->StartSound(iter->event_name.c_str());
				iter->timer_play = Time::GetGameTime();
				iter->time_to_play = Random::GetRandomFloatBetweenTwo(iter->min_time_between_plays, iter->max_time_between_plays);
			}
		}
		if (iter->instant)
		{
			if (Time::GetGameTime() - iter->timer_play_instant >= iter->time_to_play_instant && iter->spatial_place.el_emitter != nullptr && iter->can_play) {
				iter->spatial_place.el_emitter->StartSound(iter->event_name.c_str());
				iter->can_play = false;
			}
				
		}
	}
}

void TriggerEnvironment::PlayInstant()
{
	for (auto iter = env_elements.begin(); iter != env_elements.end(); ++iter)
	{
		if (iter->instant)
		{
			iter->timer_play_instant = Time::GetGameTime();
			iter->can_play = true;
		}
	}
}

void TriggerEnvironment::MoveSpatial()
{
	for (auto iter = env_elements.begin(); iter != env_elements.end(); ++iter)
	{
		if (iter->spatial_movement && iter->spatial_place.move) {
			float min_dist = 0.1f;
			iter->spatial_place.curr_move_time += Time::GetDT();

			if ((iter->spatial_place.spatial_end->transform->GetGlobalPosition() - iter->spatial_place.spatial_place->transform->GetGlobalPosition()).Length() < min_dist) {
				iter->spatial_place.move = false;
				iter->spatial_place.spatial_place->transform->SetGlobalPosition(iter->spatial_place.begin_pos);
				iter->spatial_place.curr_move_time = 0.f;
			}
			else
			{
				float time_percent = (iter->spatial_place.curr_move_time / iter->spatial_place.move_time);
				//iter->spatial_place.direction_vec = iter->spatial_place.spatial_end - iter->spatial_place_
				iter->spatial_place.spatial_place->transform->SetGlobalPosition(iter->spatial_place.spatial_place->transform->GetGlobalPosition() + (iter->spatial_place.spatial_end->transform->GetGlobalPosition() - iter->spatial_place.spatial_place->transform->GetGlobalPosition()) * (time_percent));
			}
		}
	}
}
