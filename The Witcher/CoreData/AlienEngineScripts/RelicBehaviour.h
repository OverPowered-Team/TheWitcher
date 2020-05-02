#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "DialogueManager.h"
#include "Effect.h"

class PlayerController;
class Effect;
class EventManager;
class RelicNotification;

enum (Relic_Type,
	BASE,
	ATTACK,
	DASH,
	NONE_TYPE
	);
enum (Relic_Effect,
	FIRE = 1,
	ICE = 2,
	EARTH = 3,
	LIGHTNING = 4,
	POISON = 5,
	NATURE = 6,
	SEA = 7,
	DJINN = 8,
	STRIGAS = 9,
	RAGE = 10,
	NONE_EFFECT = 0
	);

class Relic {
public:
	Relic();
	virtual ~Relic();

	virtual void OnPickUp(PlayerController* player, std::string attack = std::string());

	std::string name = "";
	std::string description = "";
	float valor = 0.0;
	float time = 0.0;
	float ticks_time = 0.0;
	Relic_Effect relic_effect = Relic_Effect::NONE_EFFECT;
	std::vector<Effect*> effects;

};

class AttackRelic : public Relic {
public:
	AttackRelic();
	virtual ~AttackRelic();

	void OnPickUp(PlayerController* player, std::string attack = std::string()) override;

	std::string attack_name = "";
	EffectData* effect_to_apply = nullptr;
};

class DashRelic : public Relic {
public:
	DashRelic();
	virtual ~DashRelic();

	void OnPickUp(PlayerController* player, std::string attack = std::string()) override;

	EffectData* effect_to_apply = nullptr;
};

class ALIEN_ENGINE_API RelicBehaviour : public Alien {

public:
	
public:

	RelicBehaviour();
	virtual ~RelicBehaviour();

	void Start();
	void Update();

	void SetRelic(const char* json_array);
	void OnTriggerEnter(ComponentCollider* collider) override;

public:

	Relic_Type relic_type = Relic_Type::NONE_TYPE;
	Relic_Effect relic_effect = Relic_Effect::NONE_EFFECT;
	Relic* relic = nullptr;

	Dialogue geraltDialogue;
	Dialogue YenneferDialogue;

};

ALIEN_FACTORY RelicBehaviour* CreateRelicBehaviour() {
	RelicBehaviour* relic_behaviour = new RelicBehaviour();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_ENUM(Relic_Type, relic_behaviour->relic_type);
	SHOW_IN_INSPECTOR_AS_ENUM(Relic_Effect, relic_behaviour->relic_effect);
	

	return relic_behaviour;
}