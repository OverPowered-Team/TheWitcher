#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API CameraUltiMode : public Alien {

	enum class  Zoom{
		NONE,
		ZOOM_OUT,
		ZOOM_IN,
		STAYING_OUT,
	};
public:

	CameraUltiMode();
	virtual ~CameraUltiMode();

	void Start();
	void Update();
	void SetDejavou(const float zoom_out, const float speed, const float staying_time);
	void DejaVou();
public:
	ComponentCamera* camera = nullptr;
	float zoom_out_fov = 0.f;
	float speed = 0.f;
	float staying_time = 0.f;
private:
	float prev_fov = 0.f;
	float difference = 0.f;
	bool is_dejavouing = false;
	float timer = 0.f;
	Zoom zoomer = Zoom::NONE;
};

ALIEN_FACTORY CameraUltiMode* CreateCameraUltiMode() {
	CameraUltiMode* alien = new CameraUltiMode();
	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->zoom_out_fov);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->speed);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(alien->staying_time);
	return alien;
}


