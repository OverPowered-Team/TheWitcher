#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

#include "Enemy.h"

//ONHIT
static void ApplyBurnOnHit(GameObject _enemy)
{
    //TODO: Add particle
    Enemy* enemy = (Enemy*)_enemy.GetComponentScript("Enemy.h");
    if (enemy && enemy->particles["p_burnonhit"])
        enemy->particles["p_burnonhit"]->SetEnable(true);
}

static void ApplyIceOnHit(GameObject _enemy)
{
    Enemy* enemy = (Enemy*)_enemy.GetComponentScript("Enemy.h");
}

static void ApplyLightningOnHit(GameObject _enemy)
{
    Enemy* enemy = (Enemy*)_enemy.GetComponentScript("Enemy.h");
}

static void ApplyPoisonOnHit(GameObject _enemy)
{
    Enemy* enemy = (Enemy*)_enemy.GetComponentScript("Enemy.h");
}

//ONDASH
static void ApplyBurnOnDash(GameObject _player)
{
    PlayerController* player = (PlayerController*)_player.GetComponentScript("PlayerController.h");
}

static void ApplyIceOnDash(GameObject _player)
{
    PlayerController* player = (PlayerController*)_player.GetComponentScript("PlayerController.h");
}

static void ApplyEarthOnDash(GameObject _player)
{
    PlayerController* player = (PlayerController*)_player.GetComponentScript("PlayerController.h");
}

static void ApplyPoisonOnDash(GameObject _player)
{
    PlayerController* player = (PlayerController*)_player.GetComponentScript("PlayerController.h");
}