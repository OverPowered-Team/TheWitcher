#include "CameraUltiMode.h"

CameraUltiMode::CameraUltiMode() : Alien()
{
}

CameraUltiMode::~CameraUltiMode()
{
}

void CameraUltiMode::Start()
{
	camera = (ComponentCamera*)Camera::GetCurrentCamera();
}

void CameraUltiMode::Update()
{
	if (Input::GetKeyDown(SDL_SCANCODE_O))
	{
		is_dejavouing = true;
		prev_fov = camera->GetVerticalFov();
	}

	if (is_dejavouing)
	{
		DejaVou();
	}
}

void CameraUltiMode::DejaVou()
{
	switch (zoomer)
	{
	case Zoom::NONE:
		zoomer = Zoom::ZOOM_OUT;
		break;
	case Zoom::ZOOM_OUT:
		difference += speed;
		camera->SetVerticalFov(Maths::Lerp(prev_fov, zoom_out_fov, Maths::Min(difference, 1.0f)));
		timer = Time::GetGameTime();
		if (difference >= 1.f) { //Staying ZoomOut
			zoomer = Zoom::STAYING_OUT;
		}
		break;
	case Zoom::ZOOM_IN:
		difference += speed;
		camera->SetVerticalFov(Maths::Lerp(zoom_out_fov, prev_fov, Maths::Min(difference, 1.0f)));
		timer = Time::GetDT();
		if (difference >= 1.f)//ZoomIn
		{
			is_dejavouing = false;
			difference = 0.f;
			zoomer = Zoom::NONE;
		}
		break;
	case Zoom::STAYING_OUT:
		if (Time::GetGameTime() - timer >= staying_time)
		{
			zoomer = Zoom::ZOOM_IN;
			difference = 0.f;
			timer = 0.f;
		}
		break;
	default:
		LOG("WHAT!?");
		break;
	}
}