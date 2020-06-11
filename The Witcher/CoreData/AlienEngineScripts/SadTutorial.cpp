#include "SadTutorial.h"

SadTutorial::SadTutorial() : Alien()
{
}

SadTutorial::~SadTutorial()
{
}

void SadTutorial::Start()
{
	emitter = game_object->GetComponent<ComponentAudioEmitter>();
	timer_explosion = Time::GetGameTime();
}

void SadTutorial::Update()
{
	if (Time::GetGameTime() - timer_explosion >= time_to_explode)
	{
		emitter->StartSound("Play_Explosion");//Explosion sound insert here
		//Animation explosion here
	}
}