#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

#include "Enemy.h"
#include "PlayerController.h"

//ONHIT
static void ApplyEffectOnHit(Enemy* _enemy, uint size, EffectData* data)
{
    Effect* effect = new Effect(data);

    if(strcmp(data->name.data(), "poison_runestone") != 0)
        effect->time = size * data->time;

    if (strcmp(data->name.data(), "poison_runestone") == 0)
    {
        for (int i = 0; i < effect->additive_modifiers.size(); ++i)
        {
            if(effect->additive_modifiers[i].identifier == "Health")
                effect->additive_modifiers[i].amount = data->additive_modifiers[i].amount * size;
        }
    }
    
    if (strcmp(data->name.data(), "lightning_runestone") == 0)
        _enemy->Stun(effect->time);

    _enemy->AddEffect(effect);
}

//ONDASH
static void ApplyEffectOnDash(Enemy* _enemy, EffectData* data)
{
    Effect* effect = new Effect(data);
    _enemy->AddEffect(effect);
}
