#include "CheckPoint.h"
#include "CheckPoints_Manager.h"

CheckPoint::CheckPoint() : Alien()
{
}

CheckPoint::~CheckPoint()
{
}

void CheckPoint::Awake()
{
	CheckPoints_Manager::checkpoints.push_back(this);
}

void CheckPoint::OnDrawGizmos()
{
	Gizmos::DrawWireSphere(this->transform->GetGlobalPosition(), 0.5f, Color::Purple());
}
