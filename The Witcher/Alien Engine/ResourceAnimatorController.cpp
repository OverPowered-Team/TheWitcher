#include "Application.h"
#include "Globals.h"
#include "Time.h"
#include "ModuleResources.h"
#include "ModuleImporter.h"
#include "ModuleInput.h"
#include "ModuleUI.h"
#include "ComponentAnimator.h"
#include "ComponentAudioEmitter.h"
#include "ComponentScript.h"
#include "ModuleFileSystem.h"
#include "ResourceAnimation.h"
#include "GameObject.h"
#include "Alien.h"
#include "ResourceScript.h"
#include <fstream>
#include <iomanip>

#include "ResourceAnimatorController.h"
#include "mmgr/mmgr.h"

ResourceAnimatorController::ResourceAnimatorController() : Resource()
{
	type = ResourceType::RESOURCE_ANIMATOR_CONTROLLER;
	ed_context = ax::NodeEditor::CreateEditor();

	default_state = nullptr;
}

ResourceAnimatorController::ResourceAnimatorController(ResourceAnimatorController* controller)
{

	name = controller->name;

	current_state = nullptr;

	for (int i = 0; i < controller->states.size(); ++i) {
		states.push_back(new State(controller->states[i]));
		if (controller->states[i] == controller->default_state)
			default_state = states[i];
	}

	for (int i = 0; i < controller->transitions.size(); ++i) {
		transitions.push_back(new Transition(controller->transitions[i], this));
	}

	int_parameters = controller->int_parameters;
	float_parameters = controller->float_parameters;
	bool_parameters = controller->bool_parameters;

	for (int i = 0; i < controller->anim_events.size(); ++i)
		anim_events.push_back(new AnimEvent(controller->anim_events[i]));


	ed_context = ax::NodeEditor::CreateEditor();

}

ResourceAnimatorController::~ResourceAnimatorController()
{
	FreeMemory();
	ax::NodeEditor::DestroyEditor(ed_context);
}

void ResourceAnimatorController::ReImport(const u64& force_id)
{
	JSON_Value* value = json_parse_file(path.data());
	JSON_Object* object = json_value_get_object(value);

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* asset = new JSONfilepack(path.data(), object, value);

		name = asset->GetString("Controller.Name");
		int num_states = asset->GetNumber("Controller.NumStates");
		int num_transitions = asset->GetNumber("Controller.NumTransitions");

		JSONArraypack* asset_states = asset->GetArray("Controller.States");
		if (asset_states)
		{
			asset_states->GetFirstNode();
			for (uint i = 0; i < asset_states->GetArraySize(); ++i)
			{
				std::string state_name = asset_states->GetString("Name");
				int state_speed = asset_states->GetNumber("Speed");
				u64 clip_id = std::stoull(asset_states->GetString("Clip"));
				AddState(state_name, clip_id == 0 ? nullptr : (ResourceAnimation*)App->resources->GetResourceWithID(clip_id), state_speed);
				asset_states->GetAnotherNode();
			}
		}
		JSONArraypack* asset_transitions = asset->GetArray("Controller.Transitions");
		if (asset_transitions)
		{
			asset_transitions->GetFirstNode();
			for (uint i = 0; i < asset_transitions->GetArraySize(); ++i)
			{
				std::string source = asset_transitions->GetString("Source");
				std::string target = asset_transitions->GetString("Target");
				float blend = asset_transitions->GetNumber("Blend");
				bool end = asset_transitions->GetBoolean("End");
				AddTransition(FindState(source), FindState(target), blend, end);

				JSONArraypack* transitions_int_conditions = asset_transitions->GetArray("IntConditions");
				if (transitions_int_conditions) {
					transitions_int_conditions->GetFirstNode();
					for (uint j = 0; j < transitions_int_conditions->GetArraySize(); ++j) {
						std::string type = transitions_int_conditions->GetString("Type");
						std::string comp_text = transitions_int_conditions->GetString("CompText");
						uint parameter_index = transitions_int_conditions->GetNumber("ParameterIndex");
						int comp_value = transitions_int_conditions->GetNumber("CompValue");

						transitions[i]->AddIntCondition(type, comp_text, parameter_index, comp_value);
						transitions_int_conditions->GetAnotherNode();
					}
				}

				JSONArraypack* transitions_float_conditions = asset_transitions->GetArray("FloatConditions");
				if (transitions_float_conditions) {
					transitions_float_conditions->GetFirstNode();
					for (uint j = 0; j < transitions_float_conditions->GetArraySize(); ++j) {
						std::string type = transitions_float_conditions->GetString("Type");
						std::string comp_text = transitions_float_conditions->GetString("CompText");
						uint parameter_index = transitions_float_conditions->GetNumber("ParameterIndex");
						float comp_value = transitions_float_conditions->GetNumber("CompValue");

						transitions[i]->AddFloatCondition(type, comp_text, parameter_index, comp_value);
						transitions_float_conditions->GetAnotherNode();
					}
				}

				JSONArraypack* transitions_bool_conditions = asset_transitions->GetArray("BoolConditions");
				if (transitions_bool_conditions) {
					transitions_bool_conditions->GetFirstNode();
					for (uint j = 0; j < transitions_bool_conditions->GetArraySize(); ++j) {
						std::string type = transitions_bool_conditions->GetString("Type");
						std::string comp_text = transitions_bool_conditions->GetString("CompText");
						uint parameter_index = transitions_bool_conditions->GetNumber("ParameterIndex");

						transitions[i]->AddBoolCondition(type, comp_text, parameter_index);
						transitions_bool_conditions->GetAnotherNode();
					}
				}
				asset_transitions->GetAnotherNode();
			}
		}

		JSONArraypack* int_parameters = asset->GetArray("Controller.IntParameters");
		if (int_parameters) {
			int_parameters->GetFirstNode();
			for (int i = 0; i < int_parameters->GetArraySize(); ++i) {
				std::string name = int_parameters->GetString("Name");
				int value = int_parameters->GetNumber("Value");

				AddIntParameter({ name, value });
				int_parameters->GetAnotherNode();
			}
		}

		JSONArraypack* float_parameters = asset->GetArray("Controller.FloatParameters");
		if (float_parameters) {
			float_parameters->GetFirstNode();
			for (int i = 0; i < float_parameters->GetArraySize(); ++i) {
				std::string name = float_parameters->GetString("Name");
				float value = float_parameters->GetNumber("Value");

				AddFloatParameter({ name, value });
				float_parameters->GetAnotherNode();
			}
		}

		JSONArraypack* bool_parameters = asset->GetArray("Controller.BoolParameters");
		if (bool_parameters) {
			bool_parameters->GetFirstNode();
			for (int i = 0; i < bool_parameters->GetArraySize(); ++i) {
				std::string name = bool_parameters->GetString("Name");
				bool value = bool_parameters->GetNumber("Value");

				AddBoolParameter({ name, value });
				bool_parameters->GetAnotherNode();
			}
		}

		JSONArraypack* events = asset->GetArray("Controller.Events");
		if (events) {
			events->GetFirstNode();
			for (int i = 0; i < events->GetArraySize(); ++i) {
				std::string event_id = events->GetString("Event_Id");
				u64 animation_id = std::stoull(events->GetString("Animation_Id"));
				uint frame = events->GetNumber("Frame");
				EventAnimType type = (EventAnimType)(uint)events->GetNumber("Type");

				anim_events.push_back(new AnimEvent(event_id, animation_id, frame, type));
				events->GetAnotherNode();
			}
		}

		CreateMetaData(ID);
		FreeMemory();
		delete asset;
	}
}

const std::vector < std::pair <std::string, bool>>& ResourceAnimatorController::GetBoolParameters()
{
	return bool_parameters;
}

const std::vector < std::pair <std::string, float>>& ResourceAnimatorController::GetFloatParameters()
{
	return float_parameters;
}

const std::vector < std::pair <std::string, int>>& ResourceAnimatorController::GetIntParameters()
{
	return int_parameters;
}

void ResourceAnimatorController::SetBoolParametersName(uint index, const std::string& name)
{
	bool_parameters[index].first = std::string(name);
}

void ResourceAnimatorController::SetFloatParametersName(uint index, const std::string& name)
{
	float_parameters[index].first = std::string(name);
}

void ResourceAnimatorController::SetIntParametersName(uint index, const std::string& name)
{
	int_parameters[index].first = std::string(name);
}

void ResourceAnimatorController::SetBoolParametersValue(uint index, const bool& value)
{
	bool_parameters[index].second = value;
}

void ResourceAnimatorController::SetFloatParametersValue(uint index, const float& value)
{
	float_parameters[index].second = value;
}

void ResourceAnimatorController::SetIntParametersValue(uint index, const int& value)
{
	int_parameters[index].second = value;
}

void ResourceAnimatorController::AddBoolParameter()
{
	bool_parameters.push_back({ "NewBool", false });
}

void ResourceAnimatorController::AddFloatParameter()
{
	float_parameters.push_back({ "NewFloat", 0.0f });
}

void ResourceAnimatorController::AddIntParameter()
{
	int_parameters.push_back({ "NewInt", 0 });
}

void ResourceAnimatorController::AddBoolParameter(std::pair<std::string, bool> param)
{
	bool_parameters.push_back(param);
}

void ResourceAnimatorController::AddFloatParameter(std::pair<std::string, float> param)
{
	float_parameters.push_back(param);
}

void ResourceAnimatorController::AddIntParameter(std::pair<std::string, int> param)
{
	int_parameters.push_back(param);
}

void ResourceAnimatorController::RemoveBoolParameter(std::string name)
{
	for (std::vector<std::pair<std::string, bool>>::iterator it = bool_parameters.begin(); it != bool_parameters.end(); ++it) {
		if ((*it).first == name) {
			for (std::vector<Transition*>::iterator t_it = transitions.begin(); t_it != transitions.end(); ++t_it) {
				std::vector<BoolCondition*> b_conditions = (*t_it)->GetBoolConditions();
				for (std::vector<BoolCondition*>::iterator c_it = b_conditions.begin(); c_it != b_conditions.end(); ++c_it) {
					if ((*c_it)->parameter_index == std::distance(bool_parameters.begin(), it)) {
						(*t_it)->RemoveBoolCondition((*c_it));
					}
				}
			}
			bool_parameters.erase(it);
			break;
		}
	}
}

void ResourceAnimatorController::RemoveFloatParameter(std::string name)
{
	for (std::vector<std::pair<std::string, float>>::iterator it = float_parameters.begin(); it != float_parameters.end(); ++it) {
		if ((*it).first == name) {
			for (std::vector<Transition*>::iterator t_it = transitions.begin(); t_it != transitions.end(); ++t_it) {
				std::vector<FloatCondition*> f_conditions = (*t_it)->GetFloatConditions();
				for (std::vector<FloatCondition*>::iterator c_it = f_conditions.begin(); c_it != f_conditions.end(); ++c_it) {
					if ((*c_it)->parameter_index == std::distance(float_parameters.begin(), it)) {
						(*t_it)->RemoveFloatCondition((*c_it));
					}
				}
			}
			float_parameters.erase(it);
			break;
		}
	}
}

void ResourceAnimatorController::RemoveIntParameter(std::string name)
{
	for (std::vector<std::pair<std::string, int>>::iterator it = int_parameters.begin(); it != int_parameters.end(); ++it) {
		if ((*it).first == name) {
			for (std::vector<Transition*>::iterator t_it = transitions.begin(); t_it != transitions.end(); ++t_it) {
				std::vector<IntCondition*> i_conditions = (*t_it)->GetIntConditions();
				for (std::vector<IntCondition*>::iterator c_it = i_conditions.begin(); c_it != i_conditions.end(); ++c_it) {
					if ((*c_it)->parameter_index == std::distance(int_parameters.begin(), it)) {
						(*t_it)->RemoveIntCondition((*c_it));
					}
				}
			}
			int_parameters.erase(it);
			break;
		}
	}
}

void ResourceAnimatorController::SetBool(std::string name, bool value)
{
	for (int i = 0; i < bool_parameters.size(); i++) {
		if (bool_parameters[i].first == name)
			bool_parameters[i].second = value;
	}
}

void ResourceAnimatorController::SetFloat(std::string name, float value)
{
	for (int i = 0; i < float_parameters.size(); i++) {
		if (float_parameters[i].first == name)
			float_parameters[i].second = value;
	}
}

void ResourceAnimatorController::SetInt(std::string name, int value)
{
	for (int i = 0; i < int_parameters.size(); i++) {
		if (int_parameters[i].first == name)
			int_parameters[i].second = value;
	}
}


void ResourceAnimatorController::Update()
{
	if (Time::IsPlaying()) {
		if (current_state)
		{
			UpdateState(current_state);
		}
	}
}

void ResourceAnimatorController::UpdateState(State* state)
{
	ResourceAnimation* animation = state->GetClip();

	if (!transitioning)CheckTriggers();

	if (animation && (animation->GetDuration()) > 0) {

		state->time += (Time::GetDT() * current_state->GetSpeed());

		if (state->time >= animation->GetDuration()) {
			if (!state->next_state) {
				std::vector<Transition*> possible_transitions = FindTransitionsFromSourceState(state);
				for (std::vector<Transition*>::iterator it = possible_transitions.begin(); it != possible_transitions.end(); ++it) {
					if ((*it)->GetBoolConditions().size() == 0 && (*it)->GetFloatConditions().size() == 0 && (*it)->GetIntConditions().size() == 0) {
						state->next_state = (*it)->GetTarget();
						state->fade_duration = (*it)->GetBlend();
						break;
					}
				}
			}
			if (state->GetClip()->loops)
			{
				state->time = 0;
				previous_key_time = current_state->GetClip()->start_tick;
			}
			else
				state->time = animation->GetDuration();
		}

	}

	if (state->next_state) {

		float to_end = state->fade_duration - state->fade_time;

		if (to_end > 0) {

			state->fade_time += (Time::GetDT());
			UpdateState(state->next_state);
		}
		else {
			if (Time::IsPlaying()) {
				auto scripts = mycomponent->game_object_attached->GetComponents<ComponentScript>();
				for (auto item = scripts.begin(); item != scripts.end(); ++item)
				{
					if ((*item)->need_alien)
					{
						Alien* alien = (Alien*)(*item)->data_ptr;
						try {
							alien->OnAnimationEnd(state->GetName().c_str());
						}
						catch (...)
						{
							LOG_ENGINE("ERROR TRYING TO CALL ANIMATION END OF SCRIPT %s", (*item)->data_name.c_str());
						}
					}
				}
			}

			current_state = state->next_state;
			state->next_state = nullptr;
			state->time = 0;
			state->fade_time = 0;
			state->fade_duration = 0;
			transitioning = false;
			previous_key_time = current_state->GetClip()->start_tick;
		}
	}
}

void ResourceAnimatorController::Stop()
{
	current_state = nullptr;
}

bool ResourceAnimatorController::CheckTriggers()
{
	bool ret = true;

	std::vector<Transition*> current_transitions = FindTransitionsFromSourceState(current_state);
	std::vector<Transition*> current_possible_transitions;

	for (std::vector<Transition*>::iterator it = current_transitions.begin(); it != current_transitions.end(); ++it) {
		bool retu = true;

		if (current_state->GetClip()) {
			if (current_state->time < current_state->GetClip()->GetDuration() && (*it)->GetEnd())
				continue;
		}

		for (int i = 0; i < (*it)->GetBoolConditions().size(); ++i) {
			if (!(*it)->GetBoolConditions()[i]->Compare(this)) {
				retu = false;
				break;
			}
		}
		for (int i = 0; i < (*it)->GetFloatConditions().size(); ++i) {
			if (!(*it)->GetFloatConditions()[i]->Compare(this)) {
				retu = false;
				break;
			}
		}
		for (int i = 0; i < (*it)->GetIntConditions().size(); ++i) {
			if (!(*it)->GetIntConditions()[i]->Compare(this)) {
				retu = false;
				break;
			}
		}

		if (retu)current_possible_transitions.push_back((*it));
	}

	if (ret && current_possible_transitions.size() > 0) {
		current_state->next_state = current_possible_transitions.front()->GetTarget();
		current_state->fade_duration = current_possible_transitions.front()->GetBlend();
		transitioning = true;
	}

	return true;

}

bool ResourceAnimatorController::SaveAsset(const u64& force_id)
{
	if (force_id == 0)
		ID = App->resources->GetRandomID();
	else
		ID = force_id;

	path = std::string(ANIM_CONTROLLER_FOLDER + name + ".animController");

	JSON_Value* asset_value = json_value_init_object();
	JSON_Object* asset_object = json_value_get_object(asset_value);
	json_serialize_to_file_pretty(asset_value, path.data());

	JSONfilepack* asset = new JSONfilepack(path.data(), asset_object, asset_value);
	asset->StartSave();
	asset->SetString("Controller.Name", name.data());
	asset->SetNumber("Controller.NumStates", states.size());
	asset->SetNumber("Controller.NumTransitions", transitions.size());

	JSONArraypack* states_array = asset->InitNewArray("Controller.States");
	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it)
	{
		states_array->SetAnotherNode();
		states_array->SetString("Name", (*it)->GetName().data());
		states_array->SetNumber("Speed", (*it)->GetSpeed());
		states_array->SetString("Clip", (*it)->GetClip() ? std::to_string((*it)->GetClip()->GetID()).data() : std::to_string(0).data());
	}

	JSONArraypack* transitions_array = asset->InitNewArray("Controller.Transitions");
	for (std::vector<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it)
	{
		transitions_array->SetAnotherNode();
		transitions_array->SetString("Source", (*it)->GetSource()->GetName().data());
		transitions_array->SetString("Target", (*it)->GetTarget()->GetName().data());
		transitions_array->SetNumber("Blend", (*it)->GetBlend());
		transitions_array->SetBoolean("End", (*it)->GetEnd());

		JSONArraypack* int_conditions_array = transitions_array->InitNewArray("IntConditions");
		std::vector<IntCondition*> int_conditions = (*it)->GetIntConditions();
		for (std::vector<IntCondition*>::iterator it_int = int_conditions.begin(); it_int != int_conditions.end(); ++it_int) {
			int_conditions_array->SetAnotherNode();
			int_conditions_array->SetString("Type", (*it_int)->type.data());
			int_conditions_array->SetString("CompText", (*it_int)->comp_text.data());
			int_conditions_array->SetNumber("ParameterIndex", (*it_int)->parameter_index);
			int_conditions_array->SetNumber("CompValue", (*it_int)->comp);
		}

		JSONArraypack* float_conditions_array = transitions_array->InitNewArray("FloatConditions");
		std::vector<FloatCondition*> float_conditions = (*it)->GetFloatConditions();
		for (std::vector<FloatCondition*>::iterator it_float = float_conditions.begin(); it_float != float_conditions.end(); ++it_float) {
			float_conditions_array->SetAnotherNode();
			float_conditions_array->SetString("Type", (*it_float)->type.data());
			float_conditions_array->SetString("CompText", (*it_float)->comp_text.data());
			float_conditions_array->SetNumber("ParameterIndex", (*it_float)->parameter_index);
			float_conditions_array->SetNumber("CompValue", (*it_float)->comp);
		}

		JSONArraypack* bool_conditions_array = transitions_array->InitNewArray("BoolConditions");
		std::vector<BoolCondition*> bool_conditions = (*it)->GetBoolConditions();
		for (std::vector<BoolCondition*>::iterator it_bool = bool_conditions.begin(); it_bool != bool_conditions.end(); ++it_bool) {
			bool_conditions_array->SetAnotherNode();
			bool_conditions_array->SetString("Type", (*it_bool)->type.data());
			bool_conditions_array->SetString("CompText", (*it_bool)->comp_text.data());
			bool_conditions_array->SetNumber("ParameterIndex", (*it_bool)->parameter_index);
		}
	}

	JSONArraypack* int_parameters_array = asset->InitNewArray("Controller.IntParameters");
	for (std::vector <std::pair <std::string, int>>::iterator it = int_parameters.begin(); it != int_parameters.end(); ++it) {
		int_parameters_array->SetAnotherNode();
		int_parameters_array->SetString("Name", (*it).first.data());
		int_parameters_array->SetNumber("Value", (*it).second);
	}

	JSONArraypack* float_parameters_array = asset->InitNewArray("Controller.FloatParameters");
	for (std::vector <std::pair <std::string, float>>::iterator it = float_parameters.begin(); it != float_parameters.end(); ++it) {
		float_parameters_array->SetAnotherNode();
		float_parameters_array->SetString("Name", (*it).first.data());
		float_parameters_array->SetNumber("Value", (*it).second);
	}

	JSONArraypack* bool_parameters_array = asset->InitNewArray("Controller.BoolParameters");
	for (std::vector <std::pair <std::string, bool>>::iterator it = bool_parameters.begin(); it != bool_parameters.end(); ++it) {
		bool_parameters_array->SetAnotherNode();
		bool_parameters_array->SetString("Name", (*it).first.data());
		bool_parameters_array->SetBoolean("Value", (*it).second);
	}

	JSONArraypack* events_array = asset->InitNewArray("Controller.Events");
	for (std::vector <AnimEvent*>::iterator it = anim_events.begin(); it != anim_events.end(); ++it) {
		events_array->SetAnotherNode();
		events_array->SetString("Event_Id", (*it)->event_id.data());
		events_array->SetString("Animation_Id", std::to_string((*it)->animation_id).data());
		events_array->SetNumber("Frame", (*it)->frame);
		events_array->SetNumber("Type", (int)(*it)->type);
	}


	asset->FinishSave();
	CreateMetaData(ID);
	delete asset;

	return true;
}

void ResourceAnimatorController::FreeMemory()
{
	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it)
	{
		if ((*it)->GetClip() && !App->IsQuiting())
			(*it)->GetClip()->DecreaseReferences();

		delete (*it);
	}
	states.clear();
	for (std::vector<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it)
	{
		std::vector<IntCondition*> int_conditions = (*it)->GetIntConditions();
		for (std::vector<IntCondition*>::iterator it_i = int_conditions.begin(); it_i != int_conditions.end(); ++it_i) {
			delete (*it_i);
		}

		std::vector<FloatCondition*> float_conditions = (*it)->GetFloatConditions();
		for (std::vector<FloatCondition*>::iterator it_f = float_conditions.begin(); it_f != float_conditions.end(); ++it_f) {
			delete (*it_f);
		}

		std::vector<BoolCondition*> bool_conditions = (*it)->GetBoolConditions();
		for (std::vector<BoolCondition*>::iterator it_b = bool_conditions.begin(); it_b != bool_conditions.end(); ++it_b) {
			delete (*it_b);
		}
		delete (*it);
	}
	transitions.clear();

	bool_parameters.clear();
	float_parameters.clear();
	int_parameters.clear();

	for (std::vector<AnimEvent*>::iterator it_anim = anim_events.begin(); it_anim != anim_events.end(); ++it_anim)
	{
		delete (*it_anim);
	}

	anim_events.clear();

	default_state = nullptr;
	current_state = nullptr;
	id_bucket = 1;
}
bool ResourceAnimatorController::LoadMemory()
{
	char* buffer;
	uint size = App->file_system->Load(meta_data_path.data(), &buffer);

	if (size > 0)
	{
		char* cursor = buffer;

		//Load name size
		uint bytes = sizeof(uint);
		uint name_size;
		memcpy(&name_size, cursor, bytes);
		cursor += bytes;

		//Load name
		bytes = name_size;
		name.resize(bytes);
		memcpy(&name[0], cursor, bytes);
		cursor += bytes;

		bytes = sizeof(uint);
		uint num_int_parameters;
		memcpy(&num_int_parameters, cursor, bytes);
		cursor += bytes;

		for (int j = 0; j < num_int_parameters; ++j) {
			//Load parameter name size
			bytes = sizeof(uint);
			memcpy(&name_size, cursor, bytes);
			cursor += bytes;

			//Load parameter name
			bytes = name_size;
			std::string tmp_param_name;
			tmp_param_name.resize(name_size);
			memcpy(&tmp_param_name[0], cursor, bytes);
			cursor += bytes;

			bytes = sizeof(int);
			int tmp_param_value;
			memcpy(&tmp_param_value, cursor, bytes);
			cursor += bytes;

			int_parameters.push_back({ tmp_param_name, tmp_param_value });
		}

		bytes = sizeof(uint);
		uint num_float_parameters;
		memcpy(&num_float_parameters, cursor, bytes);
		cursor += bytes;

		for (int j = 0; j < num_float_parameters; ++j) {
			//Load parameter name size
			bytes = sizeof(uint);
			memcpy(&name_size, cursor, bytes);
			cursor += bytes;

			//Load parameter name
			bytes = name_size;
			std::string tmp_param_name;
			tmp_param_name.resize(name_size);
			memcpy(&tmp_param_name[0], cursor, bytes);
			cursor += bytes;

			bytes = sizeof(float);
			float tmp_param_value;
			memcpy(&tmp_param_value, cursor, bytes);
			cursor += bytes;

			float_parameters.push_back({ tmp_param_name, tmp_param_value });
		}

		bytes = sizeof(uint);
		uint num_bool_parameters;
		memcpy(&num_bool_parameters, cursor, bytes);
		cursor += bytes;

		for (int j = 0; j < num_bool_parameters; ++j) {
			//Load parameter name size
			bytes = sizeof(uint);
			memcpy(&name_size, cursor, bytes);
			cursor += bytes;

			//Load parameter name
			bytes = name_size;
			std::string tmp_param_name;
			tmp_param_name.resize(name_size);
			memcpy(&tmp_param_name[0], cursor, bytes);
			cursor += bytes;

			bytes = sizeof(bool);
			bool tmp_param_value;
			memcpy(&tmp_param_value, cursor, bytes);
			cursor += bytes;

			bool_parameters.push_back({ tmp_param_name, tmp_param_value });
		}

		// Events
		bytes = sizeof(uint);
		uint num_events;
		memcpy(&num_events, cursor, bytes);
		cursor += bytes;

		for (int j = 0; j < num_events; ++j) {
			//Load parameter name size
			bytes = sizeof(uint);
			memcpy(&name_size, cursor, bytes);
			cursor += bytes;

			//Load parameter name
			bytes = name_size;
			std::string tmp_param_event;
			tmp_param_event.resize(name_size);
			memcpy(&tmp_param_event[0], cursor, bytes);
			cursor += bytes;

			bytes = sizeof(u64);
			u64 tmp_param_anim;
			memcpy(&tmp_param_anim, cursor, bytes);
			cursor += bytes;

			bytes = sizeof(uint);
			uint tmp_param_frames;
			memcpy(&tmp_param_frames, cursor, bytes);
			cursor += bytes;

			bytes = sizeof(EventAnimType);
			EventAnimType tmp_param_types;
			memcpy(&tmp_param_types, cursor, bytes);
			cursor += bytes;

			anim_events.push_back(new AnimEvent(tmp_param_event, tmp_param_anim, tmp_param_frames, tmp_param_types));
		}


		//Load transitions and states nums
		bytes = sizeof(uint);
		uint num_states;
		memcpy(&num_states, cursor, bytes);
		cursor += bytes;
		uint num_transitions;
		memcpy(&num_transitions, cursor, bytes);
		cursor += bytes;

		for (int i = 0; i < num_states; i++)
		{
			//Load name size
			uint bytes = sizeof(uint);
			name_size;
			memcpy(&name_size, cursor, bytes);
			cursor += bytes;
			//Load name
			bytes = name_size;
			std::string tmp_name;
			tmp_name.resize(name_size);
			memcpy(&tmp_name[0], cursor, bytes);
			cursor += bytes;

			//Load clip id and speed
			bytes = sizeof(u64);
			u64 clip_id;
			memcpy(&clip_id, cursor, bytes);
			cursor += bytes;
			bytes = sizeof(float);
			float speed;
			memcpy(&speed, cursor, bytes);
			cursor += bytes;

			AddState(tmp_name, clip_id == 0 ? nullptr : (ResourceAnimation*)App->resources->GetResourceWithID(clip_id), speed);
		}

		for (int i = 0; i < num_transitions; ++i)
		{
			//Load name size
			uint bytes = sizeof(uint);
			memcpy(&name_size, cursor, bytes);
			cursor += bytes;
			//Load name
			bytes = name_size;
			std::string tmp_source;
			tmp_source.resize(name_size);
			memcpy(&tmp_source[0], cursor, bytes);
			cursor += bytes;

			//Load name size
			bytes = sizeof(uint);
			memcpy(&name_size, cursor, bytes);
			cursor += bytes;
			//Load name
			bytes = name_size;
			std::string tmp_target;
			tmp_target.resize(name_size);
			memcpy(&tmp_target[0], cursor, bytes);
			cursor += bytes;

			bytes = sizeof(float);
			float tmp_blend;
			memcpy(&tmp_blend, cursor, bytes);
			cursor += bytes;

			bytes = sizeof(bool);
			bool tmp_end;
			memcpy(&tmp_end, cursor, bytes);
			cursor += bytes;

			AddTransition(FindState(tmp_source), FindState(tmp_target), tmp_blend, tmp_end);

			bytes = sizeof(uint);
			uint num_int_conditions;
			memcpy(&num_int_conditions, cursor, bytes);
			cursor += bytes;

			for (int j = 0; j < num_int_conditions; ++j) {
				//Load condition type size
				bytes = sizeof(uint);
				memcpy(&name_size, cursor, bytes);
				cursor += bytes;

				//Load condition type
				bytes = name_size;
				std::string tmp_condition_type;
				tmp_condition_type.resize(name_size);
				memcpy(&tmp_condition_type[0], cursor, bytes);
				cursor += bytes;

				//Load condition comp text size
				bytes = sizeof(uint);
				memcpy(&name_size, cursor, bytes);
				cursor += bytes;

				//Load condition comp text
				bytes = name_size;
				std::string tmp_condition_comp_text;
				tmp_condition_comp_text.resize(name_size);
				memcpy(&tmp_condition_comp_text[0], cursor, bytes);
				cursor += bytes;

				bytes = sizeof(uint);
				uint tmp_condition_param_index;
				memcpy(&tmp_condition_param_index, cursor, bytes);
				cursor += bytes;

				bytes = sizeof(int);
				int tmp_condition_comp_value;
				memcpy(&tmp_condition_comp_value, cursor, bytes);
				cursor += bytes;

				transitions[i]->AddIntCondition(tmp_condition_type, tmp_condition_comp_text, tmp_condition_param_index, tmp_condition_comp_value);
			}

			bytes = sizeof(uint);
			uint num_float_conditions;
			memcpy(&num_float_conditions, cursor, bytes);
			cursor += bytes;

			for (int j = 0; j < num_float_conditions; ++j) {
				//Load condition type size
				bytes = sizeof(uint);
				memcpy(&name_size, cursor, bytes);
				cursor += bytes;

				//Load condition type
				bytes = name_size;
				std::string tmp_condition_type;
				tmp_condition_type.resize(name_size);
				memcpy(&tmp_condition_type[0], cursor, bytes);
				cursor += bytes;

				//Load condition comp text size
				bytes = sizeof(uint);
				memcpy(&name_size, cursor, bytes);
				cursor += bytes;

				//Load condition comp text
				bytes = name_size;
				std::string tmp_condition_comp_text;
				tmp_condition_comp_text.resize(name_size);
				memcpy(&tmp_condition_comp_text[0], cursor, bytes);
				cursor += bytes;

				bytes = sizeof(uint);
				uint tmp_condition_param_index;
				memcpy(&tmp_condition_param_index, cursor, bytes);
				cursor += bytes;

				bytes = sizeof(float);
				float tmp_condition_comp_value;
				memcpy(&tmp_condition_comp_value, cursor, bytes);
				cursor += bytes;

				transitions[i]->AddFloatCondition(tmp_condition_type, tmp_condition_comp_text, tmp_condition_param_index, tmp_condition_comp_value);

			}

			bytes = sizeof(uint);
			uint num_bool_conditions;
			memcpy(&num_bool_conditions, cursor, bytes);
			cursor += bytes;

			for (int j = 0; j < num_bool_conditions; ++j) {
				//Load condition type size
				bytes = sizeof(uint);
				memcpy(&name_size, cursor, bytes);
				cursor += bytes;

				//Load condition type
				bytes = name_size;
				std::string tmp_condition_type;
				tmp_condition_type.resize(name_size);
				memcpy(&tmp_condition_type[0], cursor, bytes);
				cursor += bytes;

				//Load condition comp text size
				bytes = sizeof(uint);
				memcpy(&name_size, cursor, bytes);
				cursor += bytes;

				//Load condition comp text
				bytes = name_size;
				std::string tmp_condition_comp_text;
				tmp_condition_comp_text.resize(name_size);
				memcpy(&tmp_condition_comp_text[0], cursor, bytes);
				cursor += bytes;

				bytes = sizeof(uint);
				uint tmp_condition_param_index;
				memcpy(&tmp_condition_param_index, cursor, bytes);
				cursor += bytes;

				transitions[i]->AddBoolCondition(tmp_condition_type, tmp_condition_comp_text, tmp_condition_param_index);

			}

		}

		LOG_ENGINE("Loaded Anim Controller %s", meta_data_path.data());

		RELEASE_ARRAY(buffer);
		return true;
	}
	return false;
}

bool ResourceAnimatorController::ReadBaseInfo(const char* assets_file_path)
{
	bool ret = true;

	path = std::string(assets_file_path);
	std::string alien_path = App->file_system->GetPathWithoutExtension(assets_file_path) + "_meta.alien";

	JSON_Value* value = json_parse_file(alien_path.data());
	JSON_Object* object = json_value_get_object(value);

	if (value != nullptr && object != nullptr)
	{
		JSONfilepack* meta = new JSONfilepack(alien_path.data(), object, value);

		ID = std::stoull(meta->GetString("Meta.ID"));

		delete meta;

		meta_data_path = LIBRARY_ANIM_CONTROLLERS_FOLDER + std::to_string(ID) + ".alienAnimController";
		char* buffer;

		struct stat fileMeta;
		struct stat fileAssets;
		if (stat(meta_data_path.c_str(), &fileMeta) == 0 && stat(path.c_str(), &fileAssets) == 0) {
			if (fileAssets.st_mtime > fileMeta.st_mtime) {
				remove(meta_data_path.data());
				ReImport(GetID());
			}
		}

		uint size = App->file_system->Load(meta_data_path.data(), &buffer);

		if (size > 0)
		{
			char* cursor = buffer;

			//Load name size
			uint bytes = sizeof(uint);
			uint name_size;
			memcpy(&name_size, cursor, bytes);
			cursor += bytes;

			//Load name
			bytes = name_size;
			name.resize(bytes);
			memcpy(&name[0], cursor, bytes);
			cursor += bytes;

			//Load transitions and states nums
			bytes = sizeof(uint);
			uint num_states;
			memcpy(&num_states, cursor, bytes);
			cursor += bytes;
			uint num_transitions;
			memcpy(&num_transitions, cursor, bytes);
			cursor += bytes;
		}
		else
			return false;
	}
	else
		return false;

	App->resources->AddResource(this);

	return ret;
}

void ResourceAnimatorController::ReadLibrary(const char* meta_data)
{
	this->meta_data_path = meta_data;
	ID = std::stoull(App->file_system->GetBaseFileName(meta_data_path.data()));
	App->resources->AddResource(this);
}

bool ResourceAnimatorController::CreateMetaData(const u64& force_id)
{
	if (force_id == 0)
		ID = App->resources->GetRandomID();
	else
		ID = force_id;

	meta_data_path = std::string(LIBRARY_ANIM_CONTROLLERS_FOLDER + std::to_string(ID) + ".alienAnimController");

	//SAVE META FILE
	std::string meta_path = std::string(App->file_system->GetPathWithoutExtension(path) + "_meta.alien");

	JSON_Value* meta_value = json_value_init_object();
	JSON_Object* meta_object = json_value_get_object(meta_value);
	json_serialize_to_file_pretty(meta_value, meta_path.data());

	if (meta_value != nullptr && meta_object != nullptr)
	{
		JSONfilepack* meta = new JSONfilepack(meta_path.data(), meta_object, meta_value);
		meta->StartSave();
		meta->SetString("Meta.ID", std::to_string(ID).data());
		meta->FinishSave();
		delete meta;
	}

	//SAVE LIBRARY FILE
	uint size = sizeof(uint) + name.size() + sizeof(uint) * 6;

	for (std::vector<std::pair<std::string, int>>::iterator int_pit = int_parameters.begin(); int_pit != int_parameters.end(); ++int_pit) {
		size += sizeof(uint) + (*int_pit).first.size() + sizeof(int);
	}

	for (std::vector<std::pair<std::string, float>>::iterator float_pit = float_parameters.begin(); float_pit != float_parameters.end(); ++float_pit) {
		size += sizeof(uint) + (*float_pit).first.size() + sizeof(float);
	}

	for (std::vector<std::pair<std::string, bool>>::iterator bool_pit = bool_parameters.begin(); bool_pit != bool_parameters.end(); ++bool_pit) {
		size += sizeof(uint) + (*bool_pit).first.size() + sizeof(bool);
	}
	//AnimEvents
	for (std::vector<AnimEvent*>::iterator event_pit = anim_events.begin(); event_pit != anim_events.end(); ++event_pit) {
		size += sizeof(uint) + (*event_pit)->event_id.size() + sizeof(u64) + sizeof(uint) + sizeof(EventAnimType);
	}

	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it)
	{
		size += sizeof(uint) + (*it)->GetName().size() + sizeof(u64) + sizeof(float);
	}
	for (std::vector<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it)
	{
		size += sizeof(uint) + (*it)->GetSource()->GetName().size() + sizeof(uint) + (*it)->GetTarget()->GetName().size() + sizeof(float) + sizeof(bool) + sizeof(uint) * 3;

		std::vector<IntCondition*> int_conditions = (*it)->GetIntConditions();
		for (std::vector<IntCondition*>::iterator int_it = int_conditions.begin(); int_it != int_conditions.end(); ++int_it) {
			size += sizeof(uint) + (*int_it)->type.size() + sizeof(uint) + (*int_it)->comp_text.size() + sizeof(uint) + sizeof(int);
		}
		std::vector<FloatCondition*> float_conditions = (*it)->GetFloatConditions();
		for (std::vector<FloatCondition*>::iterator float_it = float_conditions.begin(); float_it != float_conditions.end(); ++float_it) {
			size += sizeof(uint) + (*float_it)->type.size() + sizeof(uint) + (*float_it)->comp_text.size() + sizeof(uint) + sizeof(float);
		}
		std::vector<BoolCondition*> bool_conditions = (*it)->GetBoolConditions();
		for (std::vector<BoolCondition*>::iterator bool_it = bool_conditions.begin(); bool_it != bool_conditions.end(); ++bool_it) {
			size += sizeof(uint) + (*bool_it)->type.size() + sizeof(uint) + (*bool_it)->comp_text.size() + sizeof(uint);
		}
	}
	// Allocate
	char* data = new char[size];
	char* cursor = data;
	memset(data, 0, size);

	// Store name size and name
	uint bytes = sizeof(uint);
	uint name_size = name.size();
	memcpy(cursor, &name_size, bytes);
	cursor += bytes;
	bytes = name.size();
	memcpy(cursor, name.c_str(), bytes);
	cursor += bytes;

	// Store Controller Parameters
	bytes = sizeof(uint);
	uint num_int_parameters = int_parameters.size();
	memcpy(cursor, &num_int_parameters, bytes);
	cursor += bytes;

	for (int j = 0; j < num_int_parameters; ++j) {

		//Save condition type and type size
		name_size = int_parameters[j].first.size();
		bytes = sizeof(uint);
		memcpy(cursor, &name_size, bytes);
		cursor += bytes;
		bytes = name_size;
		memcpy(cursor, int_parameters[j].first.data(), bytes);
		cursor += bytes;

		bytes = sizeof(int);
		memcpy(cursor, &int_parameters[j].second, bytes);
		cursor += bytes;
	}

	bytes = sizeof(uint);
	uint num_float_parameters = float_parameters.size();
	memcpy(cursor, &num_float_parameters, bytes);
	cursor += bytes;

	for (int j = 0; j < num_float_parameters; ++j) {

		//Save condition type and type size
		name_size = float_parameters[j].first.size();
		bytes = sizeof(uint);
		memcpy(cursor, &name_size, bytes);
		cursor += bytes;
		bytes = name_size;
		memcpy(cursor, float_parameters[j].first.data(), bytes);
		cursor += bytes;

		bytes = sizeof(float);
		memcpy(cursor, &float_parameters[j].second, bytes);
		cursor += bytes;
	}

	bytes = sizeof(uint);
	uint num_bool_parameters = bool_parameters.size();
	memcpy(cursor, &num_bool_parameters, bytes);
	cursor += bytes;

	for (int j = 0; j < num_bool_parameters; ++j) {

		//Save condition type and type size
		name_size = bool_parameters[j].first.size();
		bytes = sizeof(uint);
		memcpy(cursor, &name_size, bytes);
		cursor += bytes;
		bytes = name_size;
		memcpy(cursor, bool_parameters[j].first.data(), bytes);
		cursor += bytes;

		bytes = sizeof(bool);
		memcpy(cursor, &bool_parameters[j].second, bytes);
		cursor += bytes;
	}

	// Events
	bytes = sizeof(uint);
	uint num_events = anim_events.size();
	memcpy(cursor, &num_events, bytes);
	cursor += bytes;

	for (int j = 0; j < num_events; ++j) {

		//Save Events
		name_size = anim_events[j]->event_id.size();
		bytes = sizeof(uint);
		memcpy(cursor, &name_size, bytes);
		cursor += bytes;
		bytes = name_size;
		memcpy(cursor, anim_events[j]->event_id.data(), bytes);
		cursor += bytes;

		bytes = sizeof(u64);
		memcpy(cursor, &anim_events[j]->animation_id, bytes);
		cursor += bytes;

		bytes = sizeof(uint);
		memcpy(cursor, &anim_events[j]->frame, bytes);
		cursor += bytes;

		bytes = sizeof(EventAnimType);
		memcpy(cursor, &anim_events[j]->type, bytes);
		cursor += bytes;
	}

	//Store transitions and states nums
	bytes = sizeof(uint);
	uint num_states = states.size();
	memcpy(cursor, &num_states, bytes);
	cursor += bytes;
	uint num_transitions = transitions.size();
	memcpy(cursor, &num_transitions, bytes);
	cursor += bytes;

	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it)
	{
		// Store name size and name
		bytes = sizeof(uint);
		name_size = (*it)->GetName().size();
		memcpy(cursor, &name_size, bytes);
		cursor += bytes;
		bytes = name_size;
		memcpy(cursor, (*it)->GetName().data(), bytes);
		cursor += bytes;

		//Store clip id and speed
		bytes = sizeof(u64);
		const u64 clip_id = (*it)->GetClip() ? (*it)->GetClip()->GetID() : 0;
		memcpy(cursor, &clip_id, bytes);
		cursor += bytes;
		bytes = sizeof(float);
		float st_speed = (*it)->GetSpeed();
		memcpy(cursor, &st_speed, bytes);
		cursor += bytes;
	}

	for (std::vector<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it)
	{
		// Store name size and name
		bytes = sizeof(uint);
		name_size = (*it)->GetSource()->GetName().size();
		memcpy(cursor, &name_size, bytes);
		cursor += bytes;
		bytes = name_size;
		memcpy(cursor, (*it)->GetSource()->GetName().data(), bytes);
		cursor += bytes;

		// Store name size and name
		bytes = sizeof(uint);
		name_size = (*it)->GetTarget()->GetName().size();
		memcpy(cursor, &name_size, bytes);
		cursor += bytes;
		bytes = name_size;
		memcpy(cursor, (*it)->GetTarget()->GetName().data(), bytes);
		cursor += bytes;

		//Store blend
		bytes = sizeof(float);
		float tr_blend = (*it)->GetBlend();
		memcpy(cursor, &tr_blend, bytes);
		cursor += bytes;

		//Store end
		bytes = sizeof(bool);
		bool tr_end = (*it)->GetEnd();
		memcpy(cursor, &tr_end, bytes);
		cursor += bytes;

		//Save int Conditions
		bytes = sizeof(uint);
		uint num_int_conditions = (*it)->GetIntConditions().size();
		memcpy(cursor, &num_int_conditions, bytes);
		cursor += bytes;

		for (int j = 0; j < num_int_conditions; ++j) {

			//Save condition type and type size
			name_size = (*it)->GetIntConditions()[j]->type.size();
			bytes = sizeof(uint);
			memcpy(cursor, &name_size, bytes);
			cursor += bytes;
			bytes = name_size;
			memcpy(cursor, (*it)->GetIntConditions()[j]->type.data(), bytes);
			cursor += bytes;

			//Save condition comp text and size 
			name_size = (*it)->GetIntConditions()[j]->comp_text.size();
			bytes = sizeof(uint);
			memcpy(cursor, &name_size, bytes);
			cursor += bytes;
			bytes = name_size;
			memcpy(cursor, (*it)->GetIntConditions()[j]->comp_text.data(), bytes);
			cursor += bytes;

			bytes = sizeof(uint);
			memcpy(cursor, &(*it)->GetIntConditions()[j]->parameter_index, bytes);
			cursor += bytes;

			bytes = sizeof(int);
			memcpy(cursor, &(*it)->GetIntConditions()[j]->comp, bytes);
			cursor += bytes;

		}

		bytes = sizeof(uint);
		uint num_float_conditions = (*it)->GetFloatConditions().size();
		memcpy(cursor, &num_float_conditions, bytes);
		cursor += bytes;

		for (int j = 0; j < num_float_conditions; ++j) {

			//Save condition type and type size
			name_size = (*it)->GetFloatConditions()[j]->type.size();
			bytes = sizeof(uint);
			memcpy(cursor, &name_size, bytes);
			cursor += bytes;
			bytes = name_size;
			memcpy(cursor, (*it)->GetFloatConditions()[j]->type.data(), bytes);
			cursor += bytes;

			//Save condition comp text and size 
			name_size = (*it)->GetFloatConditions()[j]->comp_text.size();
			bytes = sizeof(uint);
			memcpy(cursor, &name_size, bytes);
			cursor += bytes;
			bytes = name_size;
			memcpy(cursor, (*it)->GetFloatConditions()[j]->comp_text.data(), bytes);
			cursor += bytes;

			bytes = sizeof(float);
			memcpy(cursor, &(*it)->GetFloatConditions()[j]->parameter_index, bytes);
			cursor += bytes;

			bytes = sizeof(float);
			memcpy(cursor, &(*it)->GetFloatConditions()[j]->comp, bytes);
			cursor += bytes;

		}

		bytes = sizeof(uint);
		uint num_bool_conditions = (*it)->GetBoolConditions().size();
		memcpy(cursor, &num_bool_conditions, bytes);
		cursor += bytes;

		for (int j = 0; j < num_bool_conditions; ++j) {

			//Save condition type and type size
			name_size = (*it)->GetBoolConditions()[j]->type.size();
			bytes = sizeof(uint);
			memcpy(cursor, &name_size, bytes);
			cursor += bytes;
			bytes = name_size;
			memcpy(cursor, (*it)->GetBoolConditions()[j]->type.data(), bytes);
			cursor += bytes;

			//Save condition comp text and size 
			name_size = (*it)->GetBoolConditions()[j]->comp_text.size();
			bytes = sizeof(uint);
			memcpy(cursor, &name_size, bytes);
			cursor += bytes;
			bytes = name_size;
			memcpy(cursor, (*it)->GetBoolConditions()[j]->comp_text.data(), bytes);
			cursor += bytes;

			bytes = sizeof(uint);
			memcpy(cursor, &(*it)->GetBoolConditions()[j]->parameter_index, bytes);
			cursor += bytes;

		}
	}

	App->file_system->Save(meta_data_path.data(), data, size);
	App->resources->AddResource(this);
	RELEASE_ARRAY(data);

	return true;
}

bool ResourceAnimatorController::DeleteMetaData()
{
	remove(meta_data_path.data());

	std::vector<Resource*>::iterator position = std::find(App->resources->resources.begin(), App->resources->resources.end(), static_cast<Resource*>(this));
	if (position != App->resources->resources.end())
		App->resources->resources.erase(position);

	delete this;

	return true;
}

void ResourceAnimatorController::Play()
{
	if (default_state)
	{
		current_state = default_state;
		ResourceAnimation* clip = current_state->GetClip();
		if (clip != nullptr)
		{
			previous_key_time = clip->start_tick;
		}
		else
		{
			LOG_ENGINE("Resource Animator COntroller %s: Clip not found", this->name);
			return;
		}
	}
}

void ResourceAnimatorController::Play(std::string state_name)
{
	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it)
	{
		if (strcmp((*it)->GetName().c_str(), state_name.c_str()) == 0) {
			current_state->next_state = nullptr;
			current_state->time = 0;
			current_state->fade_time = 0;
			current_state->fade_duration = 0;
			current_state = (*it);
			current_state->next_state = nullptr;
			current_state->time = 0;
			current_state->fade_time = 0;
			current_state->fade_duration = 0;
			transitioning = false;
			previous_key_time = current_state->GetClip()->start_tick;
			break;
		}
	}
}

bool ResourceAnimatorController::GetTransform(std::string channel_name, float3& position, Quat& rotation, float3& scale)
{
	if (current_state)
	{
		return GetTransformState(current_state, channel_name, position, rotation, scale);
	}

	return false;
}

bool ResourceAnimatorController::GetTransformState(State* state, std::string channel_name, float3& position, Quat& rotation, float3& scale)
{
	ResourceAnimation* animation = state->GetClip();

	if (animation)
	{
		uint channel_index = animation->GetChannelIndex(channel_name);

		if (channel_index < animation->num_channels) {

			float3 next_position, next_scale;
			Quat next_rotation;
			float next_key_time = 0, t = 0;

			float time_in_ticks = animation->start_tick + (state->time * animation->ticks_per_second);

			if (animation->channels[channel_index].num_position_keys > 1)
			{
				for (int i = 0; i < animation->channels[channel_index].num_position_keys; i++)
				{
					if (time_in_ticks < animation->channels[channel_index].position_keys[i + 1].time) {
						position = animation->channels[channel_index].position_keys[i].value;
						next_position = animation->channels[channel_index].position_keys[i + 1].value;
						next_key_time = animation->channels[channel_index].position_keys[i + 1].time;
						t = (float)((double)time_in_ticks / next_key_time);
						break;
					}
				}

				position = float3::Lerp(position, next_position, t);

			}
			else
				position = animation->channels[channel_index].position_keys[0].value;

			if (animation->channels[channel_index].num_rotation_keys > 1)
			{
				for (int i = 0; i < animation->channels[channel_index].num_rotation_keys; i++)
				{
					if (time_in_ticks < animation->channels[channel_index].rotation_keys[i + 1].time) {
						rotation = animation->channels[channel_index].rotation_keys[i].value;
						next_rotation = animation->channels[channel_index].rotation_keys[i + 1].value;
						next_key_time = animation->channels[channel_index].rotation_keys[i + 1].time;
						t = (float)((double)time_in_ticks / next_key_time);
						break;
					}
				}

				rotation = Quat::Slerp(rotation, next_rotation, t);


			}
			else
				rotation = animation->channels[channel_index].rotation_keys[0].value;


			if (animation->channels[channel_index].num_scale_keys > 1)
			{

				for (int i = 0; i < animation->channels[channel_index].num_scale_keys; i++)
				{
					if (time_in_ticks < animation->channels[channel_index].scale_keys[i + 1].time) {
						scale = animation->channels[channel_index].scale_keys[i].value;
						next_scale = animation->channels[channel_index].scale_keys[i + 1].value;
						next_key_time = animation->channels[channel_index].scale_keys[i + 1].time;
						t = (float)((double)time_in_ticks / next_key_time);
						break;
					}
				}

				scale = float3::Lerp(scale, next_scale, t);
			}
			else
				scale = animation->channels[channel_index].scale_keys[0].value;


			if (state->next_state) {
				float3 next_state_position, next_state_scale;
				Quat next_state_rotation;

				if (GetTransformState(state->next_state, channel_name, next_state_position, next_state_rotation, next_state_scale)) {
					float fade_t = state->fade_time / state->fade_duration;

					position = float3::Lerp(position, next_state_position, fade_t);
					rotation = Quat::Slerp(rotation, next_state_rotation, fade_t);
					scale = float3::Lerp(scale, next_state_scale, fade_t);
				}
			}

			if (next_key_time != previous_key_time)
			{
				if (next_key_time > previous_key_time + 1 && !transitioning)
				{
					for (int i = previous_key_time; i < next_key_time; ++i)
					{
						ActiveEvent(animation, i);
					}
					previous_key_time = next_key_time;
				}
				else if (next_key_time == previous_key_time + 1 && !transitioning)
				{
					ActiveEvent(animation, previous_key_time);
					previous_key_time = next_key_time;
				}
			}
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

void ResourceAnimatorController::AddState(std::string name, ResourceAnimation* clip, float speed)
{
	State* new_state = new State(name, clip);
	new_state->SetSpeed(speed);
	new_state->id = id_bucket;
	id_bucket++;
	states.push_back(new_state);

	if (!default_state)
		default_state = new_state;
}

void ResourceAnimatorController::RemoveState(std::string name)
{
	for (std::vector<Transition*>::iterator it = transitions.begin(); it != transitions.end();) {
		if ((*it)->GetSource()->GetName() == name || (*it)->GetTarget()->GetName() == name) {
			delete (*it);
			it = transitions.erase(it);
		}
		else {
			++it;
		}
	}

	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it) {
		if ((*it)->GetName() == name) {
			if ((*it)->GetClip())
				(*it)->GetClip()->DecreaseReferences();
			if ((*it) == default_state) {
				default_state = nullptr;
			}
			delete (*it);
			it = states.erase(it);
			break;
		}
	}
	if (!default_state && states.size() > 0) {
		default_state = states[0];
	}
}

State* ResourceAnimatorController::FindState(std::string name)
{
	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it) {
		if ((*it)->GetName() == name)
			return (*it);
	}
}

State* ResourceAnimatorController::FindState(uint id)
{
	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it) {
		if ((*it)->id == id)
			return (*it);
	}
}

State* ResourceAnimatorController::FindStateFromPinId(uint pin_id)
{
	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it) {
		if ((*it)->pin_in_id == pin_id || (*it)->pin_out_id == pin_id)
			return (*it);
	}
}

void ResourceAnimatorController::AddTransition(State* source, State* target, float blend, bool end)
{
	Transition* new_transition = new Transition(source, target, blend);
	new_transition->SetEnd(end);

	transitions.push_back(new_transition);
}

void ResourceAnimatorController::RemoveTransition(std::string source_name, std::string target_name)
{
	for (std::vector<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it) {
		if ((*it)->GetSource()->GetName() == source_name && (*it)->GetTarget()->GetName() == target_name) {
			delete (*it);
			it = transitions.erase(it);
			break;
		}
	}
}

// Events
AnimEvent::AnimEvent(std::string _event_id, u64 _animation_id, uint _frame, EventAnimType _type)
{
	event_id = _event_id;
	animation_id = _animation_id;
	frame = _frame;
	type = _type;
}

AnimEvent::AnimEvent(AnimEvent* anim_event)
{
	event_id = anim_event->event_id;
	animation_id = anim_event->animation_id;
	frame = anim_event->frame;
	type = anim_event->type;
}

void ResourceAnimatorController::AddAnimEvent(AnimEvent* _event)
{
	anim_events.push_back(_event);
}

void ResourceAnimatorController::RemoveAnimEvent(AnimEvent* _event)
{
	for (std::vector<AnimEvent*>::iterator it = anim_events.begin(); it != anim_events.end(); ++it)
	{
		if ((*it)->event_id == _event->event_id && (*it)->animation_id == _event->animation_id && (*it)->frame == _event->frame && (*it)->type == _event->type)
		{
			delete (*it);
			it = anim_events.erase(it);
			break;
		}
	}
}

void ResourceAnimatorController::ActiveEvent(ResourceAnimation* _animation, uint _key)
{
	for (std::vector<AnimEvent*>::iterator it = anim_events.begin(); it != anim_events.end(); ++it)
	{
		if ((*it)->animation_id == _animation->GetID() && (*it)->frame == _key)
		{
			// To Play Sound
			if ((*it)->type == EventAnimType::EVENT_AUDIO && mycomponent->game_object_attached->GetComponent(ComponentType::A_EMITTER) != nullptr)
			{
				((ComponentAudioEmitter*)mycomponent->game_object_attached->GetComponent(ComponentType::A_EMITTER))->StartSound((uint)std::stoull((*it)->event_id.c_str()));
			}

			// To Execute Script Method
			if ((*it)->type == EventAnimType::EVENT_SCRIPT)
			{
				auto scripts = mycomponent->game_object_attached->GetComponents<ComponentScript>();
				for (auto item = scripts.begin(); item != scripts.end(); ++item)
				{
					if (*item != nullptr && (*item)->data_ptr != nullptr && !(*item)->functionMap.empty())
					{
						for (auto j = (*item)->functionMap.begin(); j != (*item)->functionMap.end(); ++j) {
							if (strcmp((*j).first.data(), (*it)->event_id.c_str()) == 0)
							{
								std::function<void()> functEvent = (*j).second;
								App->objects->functions_to_call.push_back(functEvent);
							}
						}
					}
				}
			}
		}
	}
}

std::vector<Transition*> ResourceAnimatorController::FindTransitionsFromSourceState(State* state)
{
	std::vector<Transition*> ret;
	for (std::vector<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it) {
		if ((*it)->GetSource() == state) {
			ret.push_back((*it));
		}
	}

	return ret;
}

ax::NodeEditor::EditorContext* ResourceAnimatorController::GetEditorContext()
{
	return ed_context;
}

std::string ResourceAnimatorController::GetTypeString() const
{
	return "Animation Controller";
}

std::string ResourceAnimatorController::GetName()
{
	return name;
}

void ResourceAnimatorController::Reset()
{
}

State::State()
{
}

State::State(std::string name, ResourceAnimation* clip)
{
	this->name = name;
	if (clip)
		SetClip(clip);
}

State::State(State* state)
{
	name = state->name;
	speed = state->speed;
	clip = state->clip;
	if (clip)
		clip->IncreaseReferences();
	pin_in_id = state->pin_in_id;
	pin_out_id = state->pin_out_id;
	id = state->id;
	time = 0;
	fade_duration = 0;
	fade_time = 0;

	if (state->next_state != nullptr)
		next_state = new State(state->next_state);
	else next_state = nullptr;
}

void State::SetSpeed(float speed)
{
	this->speed = speed;
}

void State::SetName(std::string name)
{
	this->name = name;
}

void State::SetClip(ResourceAnimation* clip)
{
	clip->IncreaseReferences();
	this->clip = clip;
}

float State::GetSpeed()
{
	return speed;
}

std::string State::GetName()
{
	return name;
}

ResourceAnimation* State::GetClip()
{
	return clip;
}

Transition::Transition()
{

}

Transition::Transition(State* source, State* target, float blend)
{
	this->source = source;
	this->target = target;
	this->blend = blend;
}

Transition::Transition(Transition* transition, ResourceAnimatorController* controller)
{
	source = controller->FindState(transition->source->GetName());
	target = controller->FindState(transition->target->GetName());
	for (int i = 0; i < transition->int_conditions.size(); ++i)
		int_conditions.push_back(new IntCondition(transition->int_conditions[i]));
	for (int i = 0; i < transition->float_conditions.size(); ++i)
		float_conditions.push_back(new FloatCondition(transition->float_conditions[i]));
	for (int i = 0; i < transition->bool_conditions.size(); ++i)
		bool_conditions.push_back(new BoolCondition(transition->bool_conditions[i]));
	blend = transition->blend;
	end = transition->end;
}

void Transition::SetSource(State* source)
{
	this->source = source;
}

void Transition::SetTarget(State* target)
{
	this->target = target;
}


void Transition::SetBlend(float blend)
{
	this->blend = blend;
}

State* Transition::GetSource()
{
	return source;
}

State* Transition::GetTarget()
{
	return target;
}


float Transition::GetBlend()
{
	return blend;
}

void Transition::AddIntCondition()
{
	IntCondition* new_condition = new IntCondition("int", 0, 0);
	int_conditions.push_back(new_condition);
}

void Transition::AddIntCondition(std::string type, std::string comp_text, uint index, int comp)
{
	IntCondition* new_int_condition = new IntCondition(type, index, comp);
	new_int_condition->SetCompText(comp_text);
	int_conditions.push_back(new_int_condition);
}

void Transition::AddFloatCondition()
{
	FloatCondition* new_condition = new FloatCondition("float", 0, 0.0f);
	float_conditions.push_back(new_condition);
}

void Transition::AddFloatCondition(std::string type, std::string comp_text, uint index, float comp)
{
	FloatCondition* new_float_condition = new FloatCondition(type, index, comp);
	new_float_condition->SetCompText(comp_text);
	float_conditions.push_back(new_float_condition);
}

void Transition::AddBoolCondition()
{
	BoolCondition* new_condition = new BoolCondition("bool", 0);
	bool_conditions.push_back(new_condition);
}

void Transition::AddBoolCondition(std::string type, std::string comp_text, uint index)
{
	BoolCondition* new_bool_condition = new BoolCondition(type, index);
	new_bool_condition->SetCompText(comp_text);
	bool_conditions.push_back(new_bool_condition);
}

void Transition::RemoveIntCondition(IntCondition* int_condition)
{
	for (std::vector<IntCondition*>::iterator it = int_conditions.begin(); it != int_conditions.end(); ++it) {
		if ((*it) == int_condition) {
			delete (*it);
			it = int_conditions.erase(it);
			break;
		}
	}
}

void Transition::RemoveFloatCondition(FloatCondition* float_condition)
{
	for (std::vector<FloatCondition*>::iterator it = float_conditions.begin(); it != float_conditions.end(); ++it) {
		if ((*it) == float_condition) {
			delete (*it);
			it = float_conditions.erase(it);
			break;
		}
	}
}

void Transition::RemoveBoolCondition(BoolCondition* bool_condition)
{
	for (std::vector<BoolCondition*>::iterator it = bool_conditions.begin(); it != bool_conditions.end(); ++it) {
		if ((*it) == bool_condition) {
			delete (*it);
			it = bool_conditions.erase(it);
			break;
		}
	}
}

IntCondition::IntCondition(IntCondition* int_condition)
{
	type = int_condition->type;
	parameter_index = int_condition->parameter_index;
	comp_texts = int_condition->comp_texts;
	comp_text = int_condition->comp_text;
	comp = int_condition->comp;
}

bool IntCondition::Compare(ResourceAnimatorController* controller)
{
	bool ret = false;

	if (comp_text == "Equal") {
		ret = controller->GetIntParameters()[parameter_index].second == comp;
	}
	else if (comp_text == "NotEqual") {
		ret = controller->GetIntParameters()[parameter_index].second != comp;
	}
	else if (comp_text == "Greater") {
		ret = controller->GetIntParameters()[parameter_index].second > comp;
	}
	else if (comp_text == "Lesser") {
		ret = controller->GetIntParameters()[parameter_index].second < comp;
	}

	return ret;
}

FloatCondition::FloatCondition(FloatCondition* float_condition)
{
	type = float_condition->type;
	parameter_index = float_condition->parameter_index;
	comp_texts = float_condition->comp_texts;
	comp_text = float_condition->comp_text;
	comp = float_condition->comp;
}

bool FloatCondition::Compare(ResourceAnimatorController* controller)
{
	bool ret = false;

	if (comp_text == "Greater") {
		if (controller->GetFloatParameters()[parameter_index].second > comp)
			ret = true;
		else
			ret = false;
	}
	else if (comp_text == "Lesser") {
		if (controller->GetFloatParameters()[parameter_index].second < comp)
			ret = true;
		else
			ret = false;
	}

	return ret;
}

BoolCondition::BoolCondition(BoolCondition* bool_condition)
{
	type = bool_condition->type;
	parameter_index = bool_condition->parameter_index;
	comp_texts = bool_condition->comp_texts;
	comp_text = bool_condition->comp_text;
}

bool BoolCondition::Compare(ResourceAnimatorController* controller)
{
	bool ret = false;

	if (comp_text == "False") {
		if (!controller->GetBoolParameters()[parameter_index].second) ret = true;
	}
	else if (comp_text == "True") {
		if (controller->GetBoolParameters()[parameter_index].second) ret = true;
	}

	return ret;
}

Condition::Condition()
{
}


