#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class PlayerController;
class Effect;

enum Relic_Type
{
	BASE,
	ATTACK,
	DASH,
	COMPANION
};

enum Relic_Effect
{
	FIRE,
	POISON,
	EARTH,
	RANGE,
	NONE
};

class Relic {
public:
	Relic();
	virtual ~Relic();

	void OnPickUp(PlayerController* player);

	std::string name = "";
	std::string description = "";
	std::vector<Relic_Effect> relic_effects;
	std::vector<Effect*> effects;

};

class AttackRelic {
public:
	AttackRelic();
	virtual ~AttackRelic();

	void OnPickUp(PlayerController* player);

	std::string attack_name = "";

};

class ALIEN_ENGINE_API RelicBehaviour : public Alien {

public:

	RelicBehaviour();
	virtual ~RelicBehaviour();

	void Start();
	void Update();

	void OnTriggerEnter(ComponentCollider* collider) override;

	std::string name = "";
	std::string description = "";
	Relic_Type relic_type;
	std::vector<Relic_Effect> relic_effects;
	Relic* relic = nullptr;

};

ALIEN_FACTORY RelicBehaviour* CreatePlayerTest() {
	RelicBehaviour* relic_behaviour = new RelicBehaviour();
	// To show in inspector here
	//SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(relic_behaviour->speed);

	return relic_behaviour;
}