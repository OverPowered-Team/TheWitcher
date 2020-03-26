#include "RelicBehaviour.h"
#include "PlayerController.h"
#include "Effect.h"

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
	//player->PickUpRelic(this);
}

// AttackRelic
AttackRelic::AttackRelic()
{
}

AttackRelic::~AttackRelic()
{
}

void AttackRelic::OnPickUp(PlayerController* player)
{
	//List<string> attack_pool = player._playerCombos.GetFinalAttacks();
	//int random_index = Random.Range(0, attack_pool.Count);
	//attack_name = attack_pool[random_index];

	//foreach(Relic_Effect effect in relic_effects)
	//{
	//	AttackEffect test_effect = new AttackEffect();
	//	test_effect.SetAttackIdentifier(attack_name);

	//	switch (effect)
	//	{
	//	case Relic_Effect.FIRE:
	//		test_effect.on_hit_delegate = EffectFunctions.ApplyBurnOnHit;
	//		break;
	//	case Relic_Effect.POISON:
	//		break;
	//	case Relic_Effect.EARTH:
	//		test_effect.AddFlatModifer(0.1f, "Attack_Damage");
	//		break;
	//	case Relic_Effect.RANGE:
	//		test_effect.AddFlatModifer(1.5f, "Attack_Range");
	//		break;
	//	}

	//	effects.Add(test_effect);
	//}
	////description = description.Replace("_combo_", attack_name);

	//base.OnPickUp(player);
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
	case BASE:
		relic = new Relic();
		break;
	case ATTACK:
		//relic = new AttackRelic();
		break;
	case DASH:
		break;
	case COMPANION:
		break;
	default:
		break;
	}

	relic->name = name;
	relic->description = description;
	relic->relic_effects = relic_effects;
}

void RelicBehaviour::Update()
{
	
}

void RelicBehaviour::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "Geralt") == 0 || strcmp(collider->game_object_attached->GetTag(), "Yennefer") == 0)
	{
		relic->OnPickUp((PlayerController*)collider->game_object_attached->GetComponentScript("PlayerController"));
		//GameObject.Find("Canvas").GetComponent<UIManager>().CreateRelicPopup((AttackRelic)relic, relic_type);
		Destroy(this->game_object);
	}
}