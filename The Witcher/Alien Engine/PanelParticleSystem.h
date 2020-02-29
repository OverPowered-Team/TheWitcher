#pragma once

#include "Panel.h"

class ParticleSystem;

class PanelParticleSystem : public Panel
{

public:

	PanelParticleSystem(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat = SDL_SCANCODE_UNKNOWN, const SDL_Scancode& key3_repeat_extra = SDL_SCANCODE_UNKNOWN);
	~PanelParticleSystem();

	void PanelLogic() override;

	ParticleSystem* GetSystem() const;
	void SetSystem(ParticleSystem* system);

private:

	ParticleSystem* particleSystem = nullptr;
};