#include "GameManager.h"
#include "RelicBehaviour.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"
#include "Effect.h"
#include "EffectsFunctions.h"
#include "EventManager.h"
#include "Relic_Notification.h"

// Relic
Relic::Relic()
{
}

Relic::~Relic()
{
}

void Relic::OnPickUp(PlayerController* player, std::string attack)
{
	player->PickUpRelic(this);
	if(GameObject::FindWithName("InGame")->GetComponent<Relic_Notification>())
		GameObject::FindWithName("InGame")->GetComponent<Relic_Notification>()->TriggerRelic(player, this->name, this->description, attack);
}

// AttackRelic
AttackRelic::AttackRelic() : Relic()
{
}

AttackRelic::~AttackRelic()
{
}

void AttackRelic::OnPickUp(PlayerController* _player, std::string attack)
{
	std::vector<std::string> attack_pool = _player->attacks->GetFinalAttacks();

	int random_index = Random::GetRandomIntBetweenTwo(0, attack_pool.size() - 1);
	attack_name = attack_pool[random_index];

	AttackEffect* effect = new AttackEffect();
	effect->SetAttackIdentifier(attack_name);
	effect->valor = valor;
	effect->time = time;
	effect->ticks_time = ticks_time;

	switch (relic_effect)
	{
	case Relic_Effect::FIRE:
		effect->OnHit = &ApplyBurnOnHit;
		break;
	case Relic_Effect::ICE:
		effect->OnHit = &ApplyIceOnHit;
		break;
	case Relic_Effect::EARTH:
		effect->AddMultiplicativeModifier(valor, "Attack_Damage");
		break;
	case Relic_Effect::LIGHTNING:
		effect->OnHit = &ApplyLightningOnHit;
		break;
	case Relic_Effect::POISON:
		effect->OnHit = &ApplyPoisonOnHit;
		break;
	}

	effects.push_back(effect);

	Relic::OnPickUp(_player, attack_name);
}

// DashRelic
DashRelic::DashRelic() : Relic()
{
}

DashRelic::~DashRelic()
{
}

void DashRelic::OnPickUp(PlayerController* _player, std::string attack)
{
	Effect* effect = new Effect();

	switch (relic_effect)
	{
	case Relic_Effect::FIRE:
		effect->OnDash = &ApplyBurnOnDash;
		break;
	case Relic_Effect::ICE:
		effect->OnDash = &ApplyIceOnDash;
		break;
	case Relic_Effect::EARTH:
		effect->OnDash = &ApplyEarthOnDash;
		break;
	case Relic_Effect::POISON:
		effect->OnDash = &ApplyPoisonOnDash;
		break;
	}

	effects.push_back(effect);

	Relic::OnPickUp(_player);
}

// RelicBehaviour
RelicBehaviour::RelicBehaviour() : Alien()
{
}

RelicBehaviour::~RelicBehaviour()
{
}

void RelicBehaviour::Start()
{
	std::string json_str;

	switch (relic_type)
	{
	case Relic_Type::BASE:
		relic = new Relic();
		json_str = "BASE";
		break;
	case Relic_Type::ATTACK:
		relic = new AttackRelic();
		json_str = "ATTACK";
		break;
	case Relic_Type::DASH:
		relic = new DashRelic();
		json_str = "DASH";
		break;
	default:
		break;
	}

	if (relic)
	{
		relic->relic_effect = relic_effect;
		SetRelic(json_str.data());
	}

}

void RelicBehaviour::Update()
{
	
}

void RelicBehaviour::SetRelic(const char* json_array)
{
	
	JSONfilepack* relic_json = JSONfilepack::GetJSON("GameData/Relics.json");

	JSONArraypack* type_array = relic_json->GetArray(json_array);

	if (type_array)
	{
		type_array->GetFirstNode();

		for (uint i = 0; i < type_array->GetArraySize(); i++)
		{
			if (type_array->GetNumber("effect") != (int)relic_effect)
				type_array->GetAnotherNode();
			else
				break;
		}
		relic->name = type_array->GetString("name");
		relic->description = type_array->GetString("description");
	}

	JSONfilepack::FreeJSON(relic_json);
}

void RelicBehaviour::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0)
	{
		if (collider->game_object_attached->GetComponent<PlayerController>())
		{
			relic->OnPickUp(collider->game_object_attached->GetComponent<PlayerController>());

			Destroy(this->game_object);		
		}
	}
}