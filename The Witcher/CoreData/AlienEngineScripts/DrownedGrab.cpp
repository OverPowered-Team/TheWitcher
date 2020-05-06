#include "DrownedGrab.h"
#include "Drowned.h"
#include "EnemyManager.h"

DrownedGrab::DrownedGrab() : Drowned()
{
}

DrownedGrab::~DrownedGrab()
{
}

void DrownedGrab::UpdateEnemy()
{
	Enemy::UpdateEnemy();
	switch (state)
	{
	case DrownedState::IDLE:
		if (distance < stats["AttackRange"].GetValue() && distance > stats["HideDistance"].GetValue())
		{
			animator->SetFloat("speed", 0.0F);
			character_ctrl->velocity = PxExtendedVec3(0.0f, 0.0f, 0.0f);
			animator->PlayState("GetOff");
			state = DrownedState::GETOFF;
			is_hide = false;
		}
		break;

	case DrownedState::GETOFF:
		if (transform->GetGlobalScale().y < 0.006)
			transform->AddScale(float3(0.0f, 0.0001f, 0.0f));
		else
		{
			state = DrownedState::ATTACK;
			animator->PlayState("Attack");
		}
		break;


	case DrownedState::DYING:
	{
		EnemyManager* enemy_manager = GameObject::FindWithName("GameManager")->GetComponent< EnemyManager>();
		Invoke([enemy_manager, this]() -> void {enemy_manager->DeleteEnemy(this); }, 5);
		state = DrownedState::DEAD;
		animator->PlayState("Dead");
		//audio_emitter->StartSound("DrownedDeath");
	}
	}
}

void DrownedGrab::OnTriggerEnter(ComponentCollider* collider)
{
}

void DrownedGrab::Start()
{
}

void DrownedGrab::Update()
{
}