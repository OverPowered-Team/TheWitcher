#include "PanelParticleSystem.h"
#include "Application.h"
#include "ComponentParticleSystem.h"
#include "ParticleSystem.h"
#include "GameObject.h"
#include "ModuleObjects.h"
#include "ModuleCamera3D.h"
#include "Viewport.h"
#include "imgui/imgui_internal.h"

PanelParticleSystem::PanelParticleSystem(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat, const SDL_Scancode& key3_repeat_extra)
	: Panel(panel_name, key1_down, key2_repeat, key3_repeat_extra)
{
}

PanelParticleSystem::~PanelParticleSystem()
{
}

void PanelParticleSystem::PanelLogic() 
{
	if (App->objects->GetSelectedObjects().empty())
		return;

	GameObject* selected = App->objects->GetSelectedObjects().back();

	if (selected == nullptr) 
		return;

	ComponentParticleSystem* compParticleSystem = (ComponentParticleSystem*)selected->GetComponent(ComponentType::PARTICLES);

	if (compParticleSystem == nullptr)
		return;

	particleSystem = compParticleSystem->GetSystem();

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse  | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;

	

	current_viewport_pos = App->camera->scene_viewport->GetPos();
	current_viewport_size = App->camera->scene_viewport->GetSize();

	ImGui::SetNextWindowPos(ImVec2(current_viewport_pos.x + current_viewport_size.x/1.35f, current_viewport_pos.y + current_viewport_size.y/1.45f));


	if (ImGui::Begin("Particle Effect", nullptr, flags))
	{
		/*ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.8f,0.37f,0.0f,0.7f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.95f,0.5f,0.0f,0.7f });*/
		
		
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "Particle System");
		ImGui::Separator();
		ImGui::Spacing();
		static bool start = true;

		if (!start) {
			if (ImGui::Button("Play", ImVec2(75.f, 20.f))) { start = true, particleSystem->Play(); }
		}
		else
			if (ImGui::Button("Pause", ImVec2(75.f, 20.f))) { start = false, particleSystem->Pause(); }
		
		
		ImGui::SameLine();
		if (ImGui::Button("Restart", ImVec2(75.f, 20.f))) { start = true, particleSystem->Restart(); } ImGui::SameLine();
		if (ImGui::Button("Stop", ImVec2(75.f, 20.f))) { start = false, particleSystem->Stop(); }

		ImGui::Text("Playback Time: ");	ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.3f", particleSystem->emmitter.GetCurrentLife());
		ImGui::Text("Particles: ");	ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", particleSystem->GetTotalParticles());

		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "Particle Emmitter");
		ImGui::Separator();
		ImGui::Spacing();
		if (!particleSystem->isPlaying())
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

		//ImGui::PushItemWidth(55.0f);
		if (ImGui::Button("Play Emmitter", ImVec2(120.f, 20.f))) { start = true, particleSystem->StartEmmitter(); } ImGui::SameLine();
		if (ImGui::Button("Stop Emmitter", ImVec2(120.f, 20.f))) { start = false, particleSystem->StopEmmitter(); }

		if (!particleSystem->isPlaying())
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}

	}
	ImGui::End();

}

ParticleSystem* PanelParticleSystem::GetSystem() const
{
	return particleSystem;
}

void PanelParticleSystem::SetSystem(ParticleSystem* system)
{
	particleSystem = system;
}
