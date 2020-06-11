#include "CiriAuraTrigger.h"
#include "CiriAura.h"
#include "Enemy.h"

CiriAuraTrigger::CiriAuraTrigger() : Alien()
{
}

CiriAuraTrigger::~CiriAuraTrigger()
{
}

void CiriAuraTrigger::Start()
{
	Ciri_Aura = game_object->parent->GetChild("Ciri Aura");
}

void CiriAuraTrigger::OnTriggerEnter(ComponentCollider* collider)
{
	CiriAura* aura = Ciri_Aura->GetComponent<CiriAura>();

	if ((strcmp(collider->game_object_attached->GetTag(), "Player") != 0))
		return;

	if (trigger_index == 0)
		aura->state = CiriAura::AuraState::APPEAR;
	else if (trigger_index == 1)
	{
		aura->state = CiriAura::AuraState::MOVE;
		Ciri_Aura->GetComponent<ComponentAnimator>()->SetBool("Run", true);
	}
		
	game_object->ToDelete();

}
