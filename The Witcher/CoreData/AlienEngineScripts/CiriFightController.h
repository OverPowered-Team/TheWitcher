#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

#include "CiriOriginal.h"
#include "Ciri.h"

class ALIEN_ENGINE_API CiriFightController : public Alien {
public:
	std::vector<Ciri*> ciri_clones;
	float ciri_clones_scream_cd = 15.0f;
	float scream_cd_timer = 0.0f;
	bool can_mini_scream = true;

	int phase = 1;

	Prefab ciri_clone;
	std::vector<GameObject*> clone_positions;
public:

	CiriFightController();
	virtual ~CiriFightController();
	
	void Start();
	void Update();

};

ALIEN_FACTORY CiriFightController* CreateCiriFightController() {

	CiriFightController* cirifightcontroller = new CiriFightController();
	SHOW_IN_INSPECTOR_AS_PREFAB(cirifightcontroller->ciri_clone);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(cirifightcontroller->ciri_clones_scream_cd);

	return cirifightcontroller;
} 
