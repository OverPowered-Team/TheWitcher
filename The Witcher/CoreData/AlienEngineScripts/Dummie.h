#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class PlayerController;

class ALIEN_ENGINE_API Dummie : public Alien {

public:

	Dummie();
	virtual ~Dummie();
	
	void Start();
	void Update();

	void OnTriggerEnter(ComponentCollider* col) override;

	Prefab button_x;
	Prefab button_y;

private:

	void DestroyCombo();

private:
	int current_combo_number = 0;
	float time_showing = 0.0f;
	bool showing_combo = false;
	PlayerController* player = nullptr;

	std::vector<GameObject*> current_buttons;
};

ALIEN_FACTORY Dummie* CreateDummie() {
	Dummie* alien = new Dummie();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_PREFAB(alien->button_x);
	SHOW_IN_INSPECTOR_AS_PREFAB(alien->button_y);

	return alien;
} 
