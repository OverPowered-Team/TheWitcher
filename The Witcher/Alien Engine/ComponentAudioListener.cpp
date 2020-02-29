#include "ComponentAudioListener.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "ComponentTransform.h"
#include "MathGeoLib\include\MathGeoLib.h"
#include "mmgr/mmgr.h"

ComponentAudioListener::ComponentAudioListener(GameObject * parent) : Component(parent)
{
	type = ComponentType::A_LISTENER;
	listener = App->audio->CreateSoundEmitter("Listener");
	App->audio->SetListener(listener);
	//listener->SetSpacializedListener();
}

ComponentAudioListener::~ComponentAudioListener()
{
	delete listener;
	listener = nullptr;
}

void ComponentAudioListener::Update()
{
	UpdateListenerPos();
}

void ComponentAudioListener::UpdateListenerPos()
{
	ComponentTransform* transformation = game_object_attached->GetComponentTransform();
	if (transformation != nullptr && listener)
	{
		float3 pos = transformation->GetGlobalPosition();
		Quat rot = transformation->GetGlobalRotation();

		float3 up = rot.Transform(float3(0, 1, 0));
		float3 front = rot.Transform(float3(0, 0, 1));

		up.Normalize();
		front.Normalize();
		//emitter->SetPosition(-pos.x, pos.y, pos.z, -front.x, front.y, front.z, -up.x, up.y, up.z);

		listener->SetSourcePos(-pos.x, pos.y, pos.z, -front.x, front.y, front.z, -up.x, up.y, up.z);
	}
}

void ComponentAudioListener::Reset()
{
}

void ComponentAudioListener::OnEnable()
{
	listener = App->audio->CreateSoundEmitter("listener");
	App->audio->SetListener(listener);
	UpdateListenerPos();
}

void ComponentAudioListener::OnDisable()
{
	delete listener;
	listener = nullptr;
	App->audio->SetListener(listener);
}

void ComponentAudioListener::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetBoolean("Enabled", (bool)enabled);
	to_save->SetString("ID", std::to_string(ID));
	if (enabled)
		to_save->SetString("ListenerID", std::to_string(listener->GetID()));
}

void ComponentAudioListener::LoadComponent(JSONArraypack* to_load)
{
	enabled = to_load->GetBoolean("Enabled");
	ID = std::stoull(to_load->GetString("ID"));
	if(enabled)
		listener->SetListener(std::stoull(to_load->GetString("ListenerID")));
	UpdateListenerPos();
}

bool ComponentAudioListener::DrawInspector()
{
	ImGui::PushID(this);

	if (ImGui::Checkbox("##enableListener", &enabled)) {
		(enabled) ? OnEnable() : OnDisable();
	}
	ImGui::SameLine();
	if (ImGui::CollapsingHeader("Audio Listener", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen)) {
		/*ImGui::Text("Name: %s", listener->GetName());
		ImGui::Text("ID: %s", std::to_string(listener->GetID()));*/
	}
	ImGui::Separator();

	ImGui::PopID();

	return true;
}
