#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API Wagonnete_UI : public Alien {

public:

	Wagonnete_UI();
	virtual ~Wagonnete_UI();
	
	void Start();
	void Update();

public:

	void UpdateLifebar(float new_life, float max_life);

public:

	GameObject* godmode = nullptr;
	float lerping_time = 1.f;

private:

	ComponentBar* lifebar = nullptr;

	// Lerping Life
	bool has_been_hit = false;
	bool changing_life = false;
	float life_lerp_time = 0.0f;
	float life_percentatge = 0.0f;
	float actual_life = 0.0f;

};

ALIEN_FACTORY Wagonnete_UI* CreateWagonnete_UI() {
	Wagonnete_UI* alien = new Wagonnete_UI();
	// To show in inspector here

	return alien;
} 
