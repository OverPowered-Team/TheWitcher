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
    effect->time = size * data->time;
    effect->ticks_time = data->ticks_time;
    effect->last_tick_time = Time::GetGameTime();
    effect->start_time = Time::GetGameTime();
    
    for (int i = 0; i < data->additive_modifiers.size(); ++i)
    {
        effect->AddFlatModifier(data->additive_modifiers[i].amount, data->additive_modifiers[i].identifier);
    }
    for (int i = 0; i < data->multiplicative_modifiers.size(); ++i)
    {
        effect->AddMultiplicativeModifier(data->multiplicative_modifiers[i].amount, data->multiplicative_modifiers[i].identifier);
    }

    _enemy->AddEffect(effect);
}

static void ApplyIceOnHit(Enemy* _enemy, uint size)
{
    Effect* effect = new Effect();
    effect->AddMultiplicativeModifier(0.5, "Agility");
    effect->name = "Ice On Hit";
    effect->time = size * 0.2;
    effect->ticks_time = 0;
    effect->last_tick_time = Time::GetGameTime();
    effect->start_time = Time::GetGameTime();
    _enemy->AddEffect(effect);
}

static void ApplyLightningOnHit(Enemy* _enemy, uint size)
{
    
}

static void ApplyPoisonOnHit(Enemy* _enemy, uint size)
{
    Effect* effect = new Effect();
    effect->AddFlatModifier(size * 10.0f, "Health");
    effect->name = "Poison On Hit";
    effect->time = 8.0f;
    effect->ticks_time = 1.0f;
    effect->last_tick_time = Time::GetGameTime();
    effect->start_time = Time::GetGameTime();
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