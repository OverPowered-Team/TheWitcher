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
	
	go_particle = GameObject::FindWithName("CiriExplosion");
	timer_explosion = Time::GetGameTime();

}

void SadTutorial::Update()
{
	if (Time::GetGameTime() - timer_explosion >= time_to_explode)
	{
		vector<ComponentParticleSystem*> son_particle = game_object->GetComponentsInChildren<ComponentParticleSystem>();

		particle_emmitter->Play();

		for (vector<ComponentParticleSystem*>::iterator ip = son_particle.begin(); ip != son_particle.end(); ++ip)
			(*ip)->Play();
		//emitter->StartSound("Play_Explosion");
		//Explosion sound insert here
		//Animation explosion here

	}
}