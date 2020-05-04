#include "ParticlePool.h"

ParticlePool::~ParticlePool()
{
    particle_pools.clear();
}

GameObject* ParticlePool::GetInstance(std::string particle_type, float3 pos, GameObject* parent, bool local)
{
    if (particle_pools[particle_type].size() > 0)
    {
        GameObject* instance = particle_pools[particle_type].back();
        particle_pools[particle_type].erase(particle_pools[particle_type].end() - 1);

        instance->SetNewParent(parent ? parent : this->game_object);

        if(!local)
            instance->transform->SetGlobalPosition(pos);
        else
            instance->transform->SetLocalPosition(pos);

        instance->SetEnable(true);

        return instance;
    }
    else
    {
        return GameObject::Instantiate(particle_type.c_str(), pos, false, parent? parent:nullptr);
    }

    return nullptr;
}

void ParticlePool::ReleaseInstance(std::string particle_type, GameObject* instance)
{
    instance->SetNewParent(this->game_object);
    instance->SetEnable(false);
    particle_pools[particle_type].push_back(instance);
}
