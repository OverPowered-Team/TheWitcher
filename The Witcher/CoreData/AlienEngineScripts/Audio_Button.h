#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Audio_Button : public Alien {

public:

	Audio_Button();
	virtual ~Audio_Button();
	
	void Start();
	void Click();
	void Change();

	ComponentAudioEmitter* audio = nullptr;
};

ALIEN_FACTORY Audio_Button* CreateAudio_Button() {
	Audio_Button* alien = new Audio_Button();
	// To show in inspector here

	SHOW_VOID_FUNCTION(Audio_Button::Click, alien);
	SHOW_VOID_FUNCTION(Audio_Button::Change, alien);

	return alien;
} 
