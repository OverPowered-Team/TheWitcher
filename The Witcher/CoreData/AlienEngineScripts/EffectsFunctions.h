#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

#include "Enemy.h"
#include "PlayerController.h"

//ONHIT
static void ApplyEffectOnHit(Enemy* _enemy, uint size, EffectData* data)
{
    Effect* effect = new Effect();
    effect->name = data->name;
    if(strcmp(data->name.data(), "poison_runestone") == 0)
        effect->time = data->time;
    else
        effect->time = size * data->time;
    effect->ticks_time = data->ticks_time;
    effect->last_tick_time = Time::GetGameTime();
    effect->start_time = Time::GetGameTime();
    
    for (int i = 0; i < data->additive_modifiers.size(); ++i)
    {
        if (strcmp(data->name.data(), "poison_runestone") == 0)
            effect->AddFlatModifier(data->additive_modifiers[i].amount * size, data->additive_modifiers[i].identifier);
        else
            effect->AddFlatModifier(data->additive_modifiers[i].amount, data->additive_modifiers[i].identifier);
    }
    for (int i = 0; i < data->multiplicative_modifiers.size(); ++i)
    {
        effect->AddMultiplicativeModifier(data->multiplicative_modifiers[i].amount, data->multiplicative_modifiers[i].identifier);
    }
    if (strcmp(data->name.data(), "lightning_runestone") == 0)
        _enemy->AddEffect(effect);
    else
        _enemy->AddEffect(effect);
}

//ONDASH
static void ApplyBurnOnDash(PlayerController* _player)
{
}

static void ApplyIceOnDash(PlayerController* _player)
{
}

static void ApplyEarthOnDash(PlayerController* _player)
{
}

static void ApplyPoisonOnDash(PlayerController* _player)
{
}