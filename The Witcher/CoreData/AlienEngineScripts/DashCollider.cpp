#include "Enemy.h"
#include "Effect.h"
#include "DashCollider.h"
#include "Stat.h"

DashCollider::DashCollider()
{
}

DashCollider::~DashCollider()
{
}

void DashCollider::Start()
{
	std::vector<GameObject*> particle_dash = game_object->GetChildren();
	for (auto it = particle_dash.begin(); it != particle_dash.end(); ++it) {
		dash_particles.insert(std::pair((*it)->GetName(), (*it)));
		(*it)->SetEnable(false);
	}
	start_time = Time::GetGameTime();
}

void DashCollider::Update()
{
	if (start_time + effect->on_dash_effect->time < Time::GetGameTime())
		Destroy(this->game_object);
		
}

void DashCollider::OnDash(Enemy* enemy)
{
	//EFFECT ONDASH
	effect->OnDash(enemy, effect->on_dash_effect);
}

void DashCollider::OnTriggerEnter(ComponentCollider* col)
{
	if (strcmp(col->game_object_attached->GetTag(), "Enemy") == 0) 
	{
		auto comps = col->game_object_attached->GetComponents<Alien>();

		for (auto i = comps.begin(); i != comps.end(); ++i) {
			Enemy* enemy = dynamic_cast<Enemy*>(*i);
			if (enemy) {
				LOG("daño enemigo = %f", enemy->stats["Health"].GetValue());
				OnDash(enemy);
				return;
			}
		}
	}
}
