#include "GameManager.h"
#include "EffectsFactory.h"
#include "Enemy.h"
#include "Effect.h"
#include "EffectArea.h"

EffectArea::EffectArea() : Alien()
{
}

EffectArea::~EffectArea()
{
}

void EffectArea::Start()
{
	start_time = Time::GetGameTime();
}

void EffectArea::Update()
{
	if (Time::GetGameTime() - start_time >= duration)
	{
		for (auto it = enemies_afflicted.begin(); it != enemies_afflicted.end(); it++) {
			it->first->RemoveEffect(it->second);
		}
		enemies_afflicted.clear();
		GameObject::Destroy(this->game_object);
	}
}

void EffectArea::OnTriggerEnter(ComponentCollider* col)
{
	if (strcmp(col->game_object_attached->GetTag(), "Enemy") == 0) {
		Enemy* enemy = col->game_object_attached->GetComponent<Enemy>();
		if (enemy && !enemy->IsDead()) {
			Effect* new_effect = GameManager::instance->effects_factory->CreateEffect(effect_to_apply);
			enemy->AddEffect(new_effect);
			enemies_afflicted.insert(std::pair(enemy,new_effect));
		}
	}
}

void EffectArea::OnTriggerExit(ComponentCollider* col)
{
	if (strcmp(col->game_object_attached->GetTag(), "Enemy") == 0) {
		Enemy* enemy = col->game_object_attached->GetComponent<Enemy>();
		if (enemy) {
			if (auto it = enemies_afflicted.find(enemy); it != enemies_afflicted.end()) {				
				enemy->RemoveEffect(it->second);
				enemies_afflicted.erase(enemy);
			}
		}
	}
}
