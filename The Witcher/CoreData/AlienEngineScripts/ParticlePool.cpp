#include "ParticlePool.h"

ParticlePool::~ParticlePool()
{
    particle_pools.clear();
}

GameObject* ParticlePool::GetInstance(std::string particle_type, float3 pos, float3 rotation, GameObject* parent, bool local)
{
    if (particle_pools[particle_type].size() > 0)
    {
        GameObject* instance = particle_pools[particle_type].back();
        if (instance == nullptr)
        {
            LOG("Error: Particle %s failed to recover from pool.", particle_type.c_str());
            return nullptr;
        }

        particle_pools[particle_type].erase(particle_pools[particle_type].end() - 1);

        instance->SetNewParent(parent ? parent : this->game_object);

        if(!local)
            instance->transform->SetGlobalPosition(pos);
        else
            instance->transform->SetLocalPosition(pos);

        instance->transform->SetGlobalRotation(Quat::FromEulerXYZ(rotation.x, rotation.y, rotation.z));

        instance->SetEnable(true);

        return instance;
    }
    else
    {
        GameObject* instance = GameObject::Instantiate(particle_type.c_str(), pos, false, parent ? parent : nullptr);
        if (instance == nullptr)
        {
            LOG("Error: Particle %s failed to instantiate.", particle_type.c_str());
            return nullptr;
        }
        instance->transform->SetGlobalRotation(instance->parent->transform->GetGlobalRotation());

        if (!local)
            instance->transform->SetGlobalPosition(pos);
        else
            instance->transform->SetLocalPosition(pos);

        return instance;
    }

    LOG("Error: Unknown error getting particle %s", particle_type.c_str());
    return nullptr;
}

void ParticlePool::ReleaseInstance(std::string particle_type, GameObject* instance)
{
    instance->SetNewParent(this->game_object);
    instance->SetEnable(false);
    particle_pools[particle_type].push_back(instance);
}
