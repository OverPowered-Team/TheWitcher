#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

#include "Enemy.h"
#include "PlayerController.h"

//ONHIT
static void ApplyBurnOnHit(Enemy* _enemy, uint size)
{
    Effect* effect = new Effect();
    effect->AddFlatModifier(-5, "Health");
    effect->name = "Burn On Hit";
    effect->time = size * 0.5;
    effect->ticks_time = 1.0f;
    effect->last_tick_time = Time::GetGameTime();
    effect->start_time = Time::GetGameTime();
    _enemy->effects.push_back(effect);
    /*if (enemy && enemy->particles["p_burnonhit"])
        enemy->particles["p_burnonhit"]->SetEnable(true);*/

}

static void ApplyIceOnHit(Enemy* _enemy, uint size)
{
    Effect* effect = new Effect();
    effect->AddMultiplicativeModifier(0.5, "Agility");
    effect->time = size * 0.2;
    effect->ticks_time = 0;
    _enemy->effects.push_back(effect);
}

static void ApplyLightningOnHit(Enemy* _enemy, uint size)
{

}

static void ApplyPoisonOnHit(Enemy* _enemy, uint size)
{

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