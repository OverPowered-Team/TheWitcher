#include "ComponentAudioEmitter.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "ComponentTransform.h"
#include "ReturnZ.h"
#include "Time.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "mmgr/mmgr.h"

ComponentAudioEmitter::ComponentAudioEmitter(GameObject * parent) : Component(parent)
{
	type = ComponentType::A_EMITTER;
	source = App->audio->CreateSoundEmitter("Emitter");
	App->audio->emitters.push_back(this);
}

void ComponentAudioEmitter::Update()
{
	if (Time::state == Time::GameState::NONE) {
		if (play_mode) {
			play_mode = false;
		}
	}
	else if (Time::state == Time::GameState::PLAY) {
		if (!play_mode && play_on_awake) {
			Mute(mute);
			StartSound();
			play_mode = true;
		}
	}
	UpdateSourcePos();
}

ComponentAudioEmitter::~ComponentAudioEmitter()
{
	source->StopEventByName(audio_name.c_str());
	App->audio->emitters.remove(this);
	RELEASE(source);
}

void ComponentAudioEmitter::ChangeVolume(float new_volume)
{
	if (mute != true) {
		volume = new_volume;
		if (source != nullptr)
			source->SetVolume(volume);
	}
}

void ComponentAudioEmitter::Mute(bool mute)
{
	if (source != nullptr) {
		if (mute)
			source->SetVolume(0.F);
		else
			source->SetVolume(volume);
	}
	this->mute = mute;
}

void ComponentAudioEmitter::StartSound()
{
	if (source != nullptr)
		source->PlayEventByID(current_event);
}

void ComponentAudioEmitter::StartSound(uint _event)
{
	if (source != nullptr)
		source->PlayEventByID(_event);
}

void ComponentAudioEmitter::StartSound(const char* event_name)
{
	if (source != nullptr)
		source->PlayEventByName(event_name);
}

void ComponentAudioEmitter::StopSoundByName(const char* even_name)
{
	if (source != nullptr)
		source->StopEventByName(even_name);
}

void ComponentAudioEmitter::StopOwnSound()
{
	if (source != nullptr)
		source->StopEventByName(audio_name.c_str());
}

void ComponentAudioEmitter::UpdateSourcePos()
{
	ComponentTransform* transformation = game_object_attached->GetComponentTransform();

	if (transformation != nullptr)
	{
		float3 pos = transformation->GetGlobalPosition();
		Quat rot = transformation->GetGlobalRotation();

		float3 up = rot.Transform(float3(0, 1, 0));
		float3 front = rot.Transform(float3(0, 0, 1));

		up.Normalize();
		front.Normalize();

		source->SetSourcePos(-pos.x, pos.y, pos.z, -front.x, front.y, front.z, -up.x, up.y, up.z);
	}
}

void ComponentAudioEmitter::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetString("ID", std::to_string(ID).data());
	to_save->SetNumber("Type", (int)type);
	to_save->SetBoolean("Enabled", enabled);
	to_save->SetNumber("Volume", volume);
	to_save->SetBoolean("Mute", mute);
	to_save->SetString("Bank", std::to_string(current_bank).data());
	to_save->SetString("Event", std::to_string(current_event).data());
	to_save->SetBoolean("PlayOnAwake", play_on_awake);
}
void ComponentAudioEmitter::LoadComponent(JSONArraypack* to_load)
{
	ID = std::stoull(to_load->GetString("ID"));
	enabled = to_load->GetBoolean("Enabled");
	volume = to_load->GetNumber("Volume");
	mute = to_load->GetBoolean("Mute");
	play_on_awake = to_load->GetBoolean("PlayOnAwake");
	current_bank = std::stoull(to_load->GetString("Bank"));
	current_event = std::stoull(to_load->GetString("Event"));

	/*if(enabled)
		audio_name = App->audio->GetBankByID(current_bank)->events.at(current_event);*/

	auto bank = App->audio->GetBankByID(current_bank);
}
bool ComponentAudioEmitter::DrawInspector()
{
	ImGui::PushID(this);

	if (ImGui::Checkbox("##CmpActive", &enabled)) {
		ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
		if (!enabled)
			OnDisable();
		else
			OnEnable();
	}
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Audio Emitter", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen)) {
		const Bank* bk = App->audio->GetBankByID(current_bank);
		if(ImGui::BeginCombo("Banks", (bk == nullptr) ? "" : bk->name.c_str()))
		{
			auto banks = App->audio->GetBanks();
			for (auto i = banks.begin(); i != banks.end(); ++i)
			{
				bool is_selected = (bk == nullptr) ? false : (bk->id == (*i)->id);
				if (ImGui::Selectable((*i)->name.c_str(), is_selected))
				{
					source->StopEventByName(audio_name.c_str());
					current_bank = (*i)->id;
				}
					
			}
			ImGui::EndCombo();
		}

		if(bk != nullptr)
			if (ImGui::BeginCombo("Events",
				(bk == nullptr) ?
				"NONE" : (bk->events.find(current_event) != bk->events.end()) ?
				bk->events.at(current_event).c_str() : "NONE"))
			{
				for (auto i = bk->events.begin(); i != bk->events.end(); ++i)
				{
					bool is_selected = (current_event == (*i).first);
					if (ImGui::Selectable((*i).second.c_str(), is_selected))
					{
						source->StopEventByName(audio_name.c_str());
						current_event = (*i).first;
						audio_name = (*i).second;
					}
				}
				ImGui::EndCombo();
			}
		if(ImGui::Button("Play"))
		{
			StartSound();
		}
		if (ImGui::Button("Stop"))
		{
			App->audio->Stop();
			//App->audio->UnloadAllUsedBanksFromWwise(); //TODO 
		}
		ImGui::NewLine();
		if (ImGui::Checkbox("Mute", &mute)) {
			Mute(mute);
		}
		ImGui::Checkbox("PlayOnAwake", &play_on_awake);
		if (ImGui::SliderFloat("Volume", &volume, 0.F, 1.F))
			ChangeVolume(volume);
	}

	ImGui::Separator();
	ImGui::PopID();
	return true;
}

u64 ComponentAudioEmitter::GetCurrentBank()
{
	return current_bank;
}

u32 ComponentAudioEmitter::GetWwiseIDFromString(const char* Wwise_name) const
{
	return source->GetWwiseIDFromString(Wwise_name);
}

void ComponentAudioEmitter::SetSwitchState(const char* switch_group_id, const char* switch_state_id)
{
	if (source != nullptr)
		source->SetSwitch(source->GetID(), switch_group_id, switch_state_id);
}

void ComponentAudioEmitter::SetReverb(const float& strength, const char* name)
{
	if (source != nullptr)
		source->ApplyEnvReverb(strength, name);
}

void ComponentAudioEmitter::SetState(const char* state_group, const char* new_state)
{
	if (source != nullptr)
		source->ChangeState(state_group, new_state);
}

void ComponentAudioEmitter::SetRTPCValue(const char* RTPC, float value)
{
	source->SetRTPCValue(RTPC, value, source->GetID());
}

WwiseT::AudioSource* ComponentAudioEmitter::GetSource() const
{
	return source;
}

void ComponentAudioEmitter::OnEnable()
{
	Bank* bank = App->audio->GetBankByID(current_bank);
	if (bank != nullptr)
		if (bank->events.find(current_event) != bank->events.end())
			audio_name = bank->events.at(current_event);
}

void ComponentAudioEmitter::OnDisable()
{
	audio_name = "";
}

void ComponentAudioEmitter::PauseByEventName(const char* event_name)
{
	source->PauseEventByName(event_name);
}

void ComponentAudioEmitter::ResumeByEventName(const char* event_name)
{
	source->ResumeEventByName(event_name);
}