#include "Main_Menu_Music.h"

Main_Menu_Music::Main_Menu_Music() : Alien()
{
}

Main_Menu_Music::~Main_Menu_Music()
{
}

void Main_Menu_Music::Start()
{
	GetComponent<ComponentAudioEmitter>()->StartSound();
}
