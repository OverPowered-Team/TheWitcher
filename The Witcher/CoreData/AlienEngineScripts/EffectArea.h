#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class Enemy;
class Effect;

class ALIEN_ENGINE_API EffectArea : public Alien {

public:
	EffectArea();
	virtual ~EffectArea();
	
	void Start();
	void Update();

	void OnTriggerEnter(ComponentCollider* col);
	void OnTriggerExit(ComponentCollider* col);

public:
	float duration = 5.0f;
	float start_time = 0.0f;
	std::string effect_to_apply = "";

private:
	std::map<Enemy*, Effect*> enemies_afflicted;
};

ALIEN_FACTORY EffectArea* CreateEffectArea() {
	EffectArea* alien = new EffectArea();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->duration);
	SHOW_IN_INSPECTOR_AS_STRING(alien->effect_to_apply);

	return alien;
} 
