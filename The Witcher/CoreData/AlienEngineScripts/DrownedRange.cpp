#include "DrownedRange.h"
#include "EnemyManager.h"

DrownedRange::DrownedRange() : Drowned()
{
}

DrownedRange::~DrownedRange()
{
}

void DrownedRange::UpdateEnemy()
{
	Enemy::UpdateEnemy();
	switch (state)
	{
	case DrownedState::IDLE:
		if (distance < stats["VisionRange"].GetValue())
			state = DrownedState::MOVE;
		break;
	case DrownedState::MOVE:
		Move(direction);
		break;
	case DrownedState::DYING:
	{
		EnemyManager* enemy_manager = GameObject::FindWithName("GameManager")->GetComponent< EnemyManager>();
		Invoke([enemy_manager, this]() -> void {enemy_manager->DeleteEnemy(this); }, 5);
		//audio_emitter->StartSound("DrownedDeath");
	}
	}
}

void DrownedRange::Action()
{
	animator->PlayState("GetOffGround");
	state = DrownedState::GETOFF;
}

void DrownedRange::OnAnimationEnd(const char* name)
{
	if (strcmp(name, "GetOffGround") == 0)
	{
		state = DrownedState::ATTACK;
		animator->PlayState("Attack");
	}
	else if (strcmp(name, "GetOffGround") == 0)
	{

	}
}
