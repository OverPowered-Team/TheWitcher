#include "Tutorial_Intro.h"

Tutorial_Intro::Tutorial_Intro() : Alien()
{
}

Tutorial_Intro::~Tutorial_Intro()
{
}

void Tutorial_Intro::Start()
{
}

void Tutorial_Intro::Update()
{
	switch (current_state)
	{
	case Tutorial_Intro_State::DIALOGUE_1:
		break;
	case Tutorial_Intro_State::DIALOGUE_2:
		break;
	case Tutorial_Intro_State::SHAKE:
		break;
	case Tutorial_Intro_State::CAMERA_ASCENSION:
		break;
	case Tutorial_Intro_State::PARTICLE_EXPLOSION:
		break;
	case Tutorial_Intro_State::CAMERA_RETURN:
		break;
	case Tutorial_Intro_State::FINISH:
		break;
	case Tutorial_Intro_State::ANY:
		break;
	default:
		break;
	}
}