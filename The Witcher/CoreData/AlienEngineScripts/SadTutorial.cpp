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
	particle_emmitter = game_object->GetComponent<ComponentParticleSystem>();
	vector<ComponentParticleSystem*> son_particle = game_object->GetComponentsInChildren<ComponentParticleSystem>();
	particle_emmitter->Pause();

	for (vector<ComponentParticleSystem*>::iterator ip = son_particle.begin(); ip != son_particle.end(); ++ip)
		(*ip)->Pause();
	
	timer_explosion = Time::GetGameTime();
}

void SadTutorial::Update()
{
	if (Time::GetGameTime() - timer_explosion >= time_to_explode && !has_exploded)
	{
		vector<ComponentParticleSystem*> son_particle = game_object->GetComponentsInChildren<ComponentParticleSystem>();

		particle_emmitter->Play();

		for (vector<ComponentParticleSystem*>::iterator ip = son_particle.begin(); ip != son_particle.end(); ++ip)
			(*ip)->Play();

		has_exploded = true;
	}
	if (Time::GetGameTime() - timer_explosion >= time_to_explode+1 && !has_post_exploded)
	{
		emitter->StartSound("Play_Explosion");
		has_post_exploded = true;
	}


	if (Time::GetGameTime() - timer_pre_explosion >= 5 && !has_pre_exploded)
	{
		has_pre_exploded = true;
		emitter->StartSound("Play_PreExplosion");
	}
}