#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

#include "Enemy.h"
#include "PlayerController.h"

//ONHIT
static void ApplyBurnOnHit(Enemy* _enemy, uint size, Effect* effect)
{
    effect->AddFlatModifier(-effect->valor, "Health");
    effect->name = "Burn On Hit";
    effect->time = size * effect->time;
    effect->ticks_time = effect->ticks_time;
    effect->last_tick_time = Time::GetGameTime();
    effect->start_time = Time::GetGameTime();
    _enemy->AddEffect(effect);
}

static void ApplyIceOnHit(Enemy* _enemy, uint size, Effect* effect)
{
    effect->AddMultiplicativeModifier(effect->valor, "Agility");
    effect->name = "Ice On Hit";
    effect->time = size * effect->time;
    effect->ticks_time = effect->ticks_time;
    effect->last_tick_time = Time::GetGameTime();
    effect->start_time = Time::GetGameTime();
    _enemy->AddEffect(effect);
}

static void ApplyLightningOnHit(Enemy* _enemy, uint size, Effect* effect)
{
    
}

static void ApplyPoisonOnHit(Enemy* _enemy, uint size, Effect* effect)
{
    effect->AddFlatModifier(size * effect->valor, "Health");
    effect->name = "Poison On Hit";
    effect->time = effect->time;
    effect->ticks_time = effect->ticks_time;
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