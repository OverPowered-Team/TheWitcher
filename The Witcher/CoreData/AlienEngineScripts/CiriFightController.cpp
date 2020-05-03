#include "CiriFightController.h"

CiriFightController::CiriFightController() : Alien()
{
}

CiriFightController::~CiriFightController()
{
}

void CiriFightController::Start()
{
}

void CiriFightController::Update()
{
	if (scream_cd_timer < ciri_clones_scream_cd) {
		scream_cd_timer += Time::GetDT();
	}
	else {
		can_mini_scream = true;
		scream_cd_timer = 0;
	}
}