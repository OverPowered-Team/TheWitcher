#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API ParticlePool : public Alien {

public:
	ParticlePool() {}
	virtual ~ParticlePool();

	GameObject* GetInstance(std::string particle_type, float3 pos = float3::zero(), float3 rotation = float3::zero(), GameObject* parent = nullptr, bool local = false);
	void ReleaseInstance(std::string particle_type, GameObject* instance);

private:
	std::map<std::string, std::vector<GameObject*>> particle_pools;


};

ALIEN_FACTORY ParticlePool* CreateParticlePool() {
	ParticlePool* alien = new ParticlePool();
	// To show in inspector here

	return alien;
}

