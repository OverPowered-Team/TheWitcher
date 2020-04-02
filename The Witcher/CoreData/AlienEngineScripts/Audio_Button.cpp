#include "Audio_Button.h"

Audio_Button::Audio_Button() : Alien()
{
}

Audio_Button::~Audio_Button()
{
}

void Audio_Button::Start()
{
	audio = (ComponentAudioEmitter*)game_object->GetComponent(ComponentType::A_EMITTER);
}

void Audio_Button::Click()
{
	audio->StartSound("Play_MenuSelect");
}

void Audio_Button::Change()
{
	audio->StartSound("Play_MenuMove");
}