#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class DashEffect;
class Enemy;

class ALIEN_ENGINE_API DashCollider : public Alien {
public:
	DashCollider();
	virtual ~DashCollider();

	void Start();
	void Update();

	void OnDash(Enemy* enemy);
	void OnTriggerEnter(ComponentCollider* col);


	DashEffect* effect = nullptr;
	float start_time = 0.0;
	std::map<std::string, GameObject*> dash_particles;
	
};

ALIEN_FACTORY DashCollider* CreateDashCollider() {
	DashCollider* dash_collider = new DashCollider();
	// To show in inspector here

	return dash_collider;
}



