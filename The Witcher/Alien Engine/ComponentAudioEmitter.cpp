#include "ComponentAudioEmitter.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "ComponentTransform.h"
#include "ReturnZ.h"
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
		source->SetVolume(volume);
	}
}

void ComponentAudioEmitter::Mute(bool mute)
{
	if (mute)
		source->SetVolume(0.F);
	else
		source->SetVolume(volume);
	this->mute = mute;
}

void ComponentAudioEmitter::StartSound()
{	
	source->PlayEventByID(current_event);
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
	to_save->SetString("ID", std::to_string(ID));
	to_save->SetNumber("Type", (int)type);
	to_save->SetBoolean("Enabled", enabled);
	to_save->SetNumber("Volume", volume);
	to_save->SetBoolean("Mute", mute);
	to_save->SetString("Bank", std::to_string(current_bank));
	to_save->SetString("Event", std::to_string(current_event));
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
	if (!AlreadyUsedBank(bank))
		App->audio->used_banks.push_back(bank);
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
					current_bank = (*i)->id;

					if(!AlreadyUsedBank((*i)))
						App->audio->used_banks.push_back((*i));
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
						current_event = (*i).first;
						audio_name = (*i).second;
					}
				}
				ImGui::EndCombo();
			}
		if(ImGui::Button("Play"))
		{
			App->audio->LoadUsedBanks();
			StartSound();
		}
		if (ImGui::Button("Stop"))
		{
			App->audio->Stop();
			App->audio->UnloadAllUsedBanksFromWwise();
		}			
		ImGui::NewLine();
		ImGui::Checkbox("Mute", &mute);
		ImGui::Checkbox("PlayOnAwake", &play_on_awake);
		ImGui::Checkbox("Loop", &loop);
		if (ImGui::SliderFloat("Volume", &volume, 0.F, 1.F))
			ChangeVolume(volume);
	}

	ImGui::Separator();
	ImGui::PopID();
	return true;
}

bool ComponentAudioEmitter::AlreadyUsedBank(const Bank* bk)
{
	for (auto i = App->audio->used_banks.begin(); i != App->audio->used_banks.end(); ++i)
	{
		if (current_bank == (*i)->id)
			return true;
	}

	return false;
}

void ComponentAudioEmitter::OnEnable()
{
	audio_name = App->audio->GetBankByID(current_bank)->events.at(current_event);
}

void ComponentAudioEmitter::OnDisable()
{
	audio_name = "";
}
