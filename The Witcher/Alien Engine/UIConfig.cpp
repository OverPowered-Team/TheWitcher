#include "UIConfig.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ComponentCheckbox.h"
#include "ComponentAudioEmitter.h"
#include "GameObject.h"

bool UIConfig::fullscreen = true;
float UIConfig::volume = 50.0f;

void UIConfig::AddVolume(ComponentAudioEmitter* emitter)
{
	if (emitter == nullptr)
		return;

	volume += 5.0f;
	if (volume >= 100.0f)
		volume = 100.0f;

	emitter->SetRTPCValue("GeneralVolumes", volume);
}

void UIConfig::SubtractVolume(ComponentAudioEmitter* emitter)
{
	if (emitter == nullptr)
		return;

	volume -= 5.0f;
	if (volume <= 0.0f)
		volume = 0.0f;

	emitter->SetRTPCValue("GeneralVolumes", volume);
}

void UIConfig::FullScreen()
{
	fullscreen = !fullscreen;
	App->window->SetFullScreen(fullscreen);

}
