#include "CiriDeadZone.h"
#include "Enemy.h"
#include "Boss.h"

CiriDeadZone::CiriDeadZone() : Alien()
{
}

CiriDeadZone::~CiriDeadZone()
{
}

void CiriDeadZone::Start()
{
}

void CiriDeadZone::Update()
{
}

void CiriDeadZone::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "Enemy") == 0) {
		Enemy* enemy = collider->game_object_attached->GetComponent<Enemy>();
		if (enemy && enemy->game_object->GetComponent<Boss>()->state != Boss::BossState::DEAD) {
			enemy->GetDamaged(200);
		}
	}
}
