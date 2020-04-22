#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"


enum (RumblerType,
	RECEIVE_HIT,
	HEAVY_ATTACK,
	REVIVE,
	INCREASING,
	DECREASING,
	NONE
	);

class ALIEN_ENGINE_API RumblerManager : public Alien {

public:
	RumblerManager();
	~RumblerManager();

	void Start();
	void Update();

	//Rumbler
	void StartRumbler(RumblerType type, int index_controller = 0, float rumbler_time = 0.0f);
	float increasing = 0.0f;
	float decreasing = 1.0f;
	float rumbler_start_time = 0.0f;
	float rumbler_end_time = 0.0f;
	int index_controller = 0;

};

ALIEN_FACTORY RumblerManager* CreateRumblerManager() {
	RumblerManager* rumbler = new RumblerManager();
	// To show in inspector here

	return rumbler;
}

