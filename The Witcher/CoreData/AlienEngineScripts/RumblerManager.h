#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"


enum (RumblerType,
	RECEIVE_HIT,
	HEAVY_ATTACK,
	LAST_ATTACK,
	REVIVE,
	INCREASING,
	DECREASING,
	NONE
	);

class Rumbler {
public:
	Rumbler();
	~Rumbler();

	std::string name = "";
	float strength = 0.0f;
	float duration = 0.0f;
	RumblerType type = RumblerType::NONE;

};

class ALIEN_ENGINE_API RumblerManager : public Alien {

public:
	RumblerManager();
	~RumblerManager();

	void Start();
	void Update();
	void CleanUp();

	Rumbler* GetRumbler(RumblerType _type);
	void FillRumblers(const char* _json_path);
	RumblerType GetType(std::string _name_type);
	void CalculateTime(Rumbler* _rumbler, float _increase_decrease_time);

	//Rumbler
	void StartRumbler(RumblerType _type, int _index_controller = 0, float _increase_decrease_time = 0.0f);
	void StartRumbler(std::string _name, int _index_controller);
	float rumbler_start_time = 0.0f;
	float rumbler_end_time = 0.0f;
	bool increase_decrease = false;
	int index_controller = 0;

	std::vector<Rumbler*> rumblers;

};

ALIEN_FACTORY RumblerManager* CreateRumblerManager() {
	RumblerManager* rumbler = new RumblerManager();
	// To show in inspector here

	return rumbler;
}

