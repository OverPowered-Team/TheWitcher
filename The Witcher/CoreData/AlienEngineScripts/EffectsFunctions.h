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
    if (enemy && enemy->particles["p_iceonhit"])
        enemy->particles["p_iceonhit"]->SetEnable(true);
}

static void ApplyLightningOnHit(GameObject _enemy)
{
    Enemy* enemy = (Enemy*)_enemy.GetComponentScript("Enemy.h");
    if(enemy && enemy->particles["p_lightonhit"])
        enemy->particles["p_lightonhit"]->SetEnable(true);
}

static void ApplyPoisonOnHit(GameObject _enemy)
{
    Enemy* enemy = (Enemy*)_enemy.GetComponentScript("Enemy.h");
    if (enemy && enemy->particles["p_poisononhit"])
        enemy->particles["p_poisononhit"]->SetEnable(true);
}

//ONDASH
static void ApplyBurnOnDash(GameObject _player)
{
    PlayerController* player = (PlayerController*)_player.GetComponentScript("PlayerController.h");
    player->particles["p_burnondash"]->SetEnable(true);
}

static void ApplyIceOnDash(GameObject _player)
{
    PlayerController* player = (PlayerController*)_player.GetComponentScript("PlayerController.h");
    player->particles["p_iceondash"]->SetEnable(true);
}

static void ApplyEarthOnDash(GameObject _player)
{
    PlayerController* player = (PlayerController*)_player.GetComponentScript("PlayerController.h");
    player->particles["p_earthondash"]->SetEnable(true);
}

static void ApplyPoisonOnDash(GameObject _player)
{
    PlayerController* player = (PlayerController*)_player.GetComponentScript("PlayerController.h");
    player->particles["p_poisonondash"]->SetEnable(true);
}