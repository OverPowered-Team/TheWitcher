#include "Audio.h"

Audio::Audio() : Alien()
{
}

Audio::~Audio()
{
}

void Audio::Start()
{
	audio = (ComponentAudioEmitter*)GetComponent(ComponentType::A_EMITTER);
}

void Audio::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_A))
		audio->StartSound();
}