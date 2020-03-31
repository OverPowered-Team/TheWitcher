#include "RelicBehaviour.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"
#include "Effect.h"
#include "EffectsFunctions.h"
#include "EventManager.h"

// Relic
Relic::Relic()
{
	// Inicialize effects with a json
}

Relic::~Relic()
{
}

void Relic::OnPickUp(PlayerController* player)
{
	player->PickUpRelic(this);
}

// AttackRelic
AttackRelic::AttackRelic() : Relic()
{
}

AttackRelic::~AttackRelic()
{
}

void AttackRelic::OnPickUp(PlayerController* _player)
{
	std::vector<std::string> attack_pool = _player->attacks->GetFinalAttacks();

	int random_index = Random::GetRandomIntBetweenTwo(0, attack_pool.size() - 1);
	attack_name = attack_pool[random_index];

	AttackEffect* test_effect = new AttackEffect();
	test_effect->SetAttackIdentifier(attack_name);

	switch (relic_effect)
	{
	case Relic_Effect::FIRE:
		test_effect->OnHit = &ApplyBurnOnHit;
		break;
	case Relic_Effect::POISON:
		break;
	case Relic_Effect::EARTH:
		test_effect->AddFlatModifier(0.1f, "Attack_Damage");
		break;
	case Relic_Effect::RANGE:
		test_effect->AddFlatModifier(1.5f, "Attack_Damage");
		break;
	}

	effects.push_back(test_effect);

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

	switch (relic_type)
	{
	case Relic_Type::BASE:
		relic = new Relic();
		break;
	case Relic_Type::ATTACK:
		relic = new AttackRelic();
		break;
	case Relic_Type::DASH:
		break;
	case Relic_Type::COMPANION:
		break;
	default:
		break;
	}

	if (relic)
	{
		relic->name = name;
		relic->description = description;
		relic->relic_effect = relic_effect;
	}

	eventManager = (EventManager*)GameObject::FindWithName("EventManager")->GetComponentScript("EventManager");
	geraltDialogue.audioData.eventName = "Hit_Sword";
	geraltDialogue.priority = "Relics";
}

void RelicBehaviour::Update()
{
	
}

void RelicBehaviour::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0)
	{
		if ((PlayerController*)collider->game_object_attached->GetComponentScript("PlayerController"))
		{
			relic->OnPickUp((PlayerController*)collider->game_object_attached->GetComponentScript("PlayerController"));
			//GameObject.Find("Canvas").GetComponent<UIManager>().CreateRelicPopup((AttackRelic)relic, relic_type);
			Destroy(this->game_object);

			eventManager->ReceiveDialogueEvent(geraltDialogue);
		}
	}
}