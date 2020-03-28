#include "CameraLimits.h"

CameraLimits::CameraLimits() : Alien()
{
}

CameraLimits::~CameraLimits()
{
}

void CameraLimits::Start()
{

}

void CameraLimits::Update()
{
	
}

void CameraLimits::OnDrawGizmos()
{
    static float3 points[8];
    ComponentCamera* cam = (ComponentCamera*)Camera::GetCurrentCamera()->game_object_attached->GetComponent(ComponentType::CAMERA);
    cam->frustum.GetCornerPoints(points);
    Gizmos::DrawLine(points[0], points[1], Color::Blue(), 1.f);
    Gizmos::DrawLine(points[0], points[2], Color::Blue(), 1.f);
    Gizmos::DrawLine(points[0], points[4], Color::Blue(), 1.f);
    Gizmos::DrawLine(points[2], points[6], Color::Blue(), 1.f);
    Gizmos::DrawLine(points[2], points[3], Color::Blue(), 1.f);
    Gizmos::DrawLine(points[4], points[6], Color::Blue(), 1.f);
    Gizmos::DrawLine(points[4], points[5], Color::Blue(), 1.f);
    Gizmos::DrawLine(points[6], points[7], Color::Blue(), 1.f);
    Gizmos::DrawLine(points[5], points[7], Color::Blue(), 1.f);
    Gizmos::DrawLine(points[3], points[7], Color::Blue(), 1.f);
    Gizmos::DrawLine(points[3], points[1], Color::Blue(), 1.f);
    Gizmos::DrawLine(points[1], points[5], Color::Blue(), 1.f);
}
