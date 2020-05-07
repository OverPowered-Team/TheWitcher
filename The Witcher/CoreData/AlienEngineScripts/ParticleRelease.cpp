#include "GameManager.h"
#include "ParticlePool.h"
#include "ParticleRelease.h"

ParticleRelease::ParticleRelease() : Alien()
{
}

ParticleRelease::~ParticleRelease()
{
}

void ParticleRelease::Start()
{
}

void ParticleRelease::Update()
{
	if (this->game_object->IsEnabled() && !p_system->GetSystem()->isPlaying())
		GameManager::instance->particle_pool->ReleaseInstance(this->game_object->GetName(), this->game_object);
}