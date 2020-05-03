#include "ParticlePool.h"

ParticlePool::~ParticlePool()
{
    particle_pools.clear();
}

GameObject* ParticlePool::GetInstance(std::string particle_type, float3 pos, GameObject* parent, bool local)
{
    GameObject* instance = nullptr;

    if (particle_pools[particle_type].size() > 0)
    {
        instance = particle_pools[particle_type].back();
        particle_pools[particle_type].erase(particle_pools[particle_type].end() - 1);

        if (instance->parent != parent)
            instance->SetNewParent(parent);

        if(!local)
            instance->transform->SetGlobalPosition(pos);
        else
            instance->transform->SetLocalPosition(pos);

        instance->SetEnable(true);
    }
    else
    {
        instance = GameObject::Instantiate(particle_type.c_str(), pos, false, parent? parent:nullptr);
    }

    return instance;
}

void ParticlePool::ReleaseInstance(std::string particle_type, GameObject* instance)
{
    instance->SetEnable(false);
    particle_pools[particle_type].push_back(instance);
}
