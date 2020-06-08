#include "Tutorial_HUD.h"

Tutorial_HUD::Tutorial_HUD() : Alien()
{
}

Tutorial_HUD::~Tutorial_HUD()
{
}

void Tutorial_HUD::ShowTriggerAttack(bool show)
{
	if (show && (current_state_attack == Current_Showing::ANY || current_state_attack == Current_Showing::FADING_OUT))
	{
		if (current_state_attack == Current_Showing::FADING_OUT)
		{
			attack_time = attack_time - (Time::GetTimeSinceStart() - attack_time);
		}
		else
		{
			attack_time = Time::GetTimeSinceStart();
		}

		current_state_attack = Current_Showing::FADING_IN;
	}
	else if (!show && (current_state_attack == Current_Showing::FADING_IN || current_state_attack == Current_Showing::SHOWING))
	{
		if (current_state_attack == Current_Showing::FADING_IN)
		{
			attack_time = attack_time - (Time::GetTimeSinceStart() - attack_time);
		}
		else
		{
			attack_time = Time::GetTimeSinceStart();
		}

		current_state_attack = Current_Showing::FADING_OUT;
	}
}

void Tutorial_HUD::ShowTriggerDash(bool show)
{
	if (show && (current_state_dash == Current_Showing::ANY || current_state_dash == Current_Showing::FADING_OUT))
	{
		if (current_state_dash == Current_Showing::FADING_OUT)
		{
			dash_time = dash_time - (Time::GetTimeSinceStart() - dash_time);
		}
		else
		{
			dash_time = Time::GetTimeSinceStart();
		}

		current_state_dash = Current_Showing::FADING_IN;
	}
	else if (!show && (current_state_dash == Current_Showing::FADING_IN || current_state_dash == Current_Showing::SHOWING))
	{
		if (current_state_dash == Current_Showing::FADING_IN)
		{
			dash_time = dash_time - (Time::GetTimeSinceStart() - dash_time);
		}
		else
		{
			dash_time = Time::GetTimeSinceStart();
		}

		current_state_dash = Current_Showing::FADING_OUT;
	}
}

void Tutorial_HUD::ShowTriggerMagic(bool show)
{
	if (show && (current_state_magic == Current_Showing::ANY || current_state_magic == Current_Showing::FADING_OUT))
	{
		if (current_state_magic == Current_Showing::FADING_OUT)
		{
			magic_time = magic_time - (Time::GetTimeSinceStart() - magic_time);
		}
		else
		{
			magic_time = Time::GetTimeSinceStart();
		}

		current_state_magic = Current_Showing::FADING_IN;
	}
	else if (!show && (current_state_magic == Current_Showing::FADING_IN || current_state_magic == Current_Showing::SHOWING))
	{
		if (current_state_magic == Current_Showing::FADING_IN)
		{
			magic_time = magic_time - (Time::GetTimeSinceStart() - magic_time);
		}
		else
		{
			magic_time = Time::GetTimeSinceStart();
		}

		current_state_magic = Current_Showing::FADING_OUT;
	}
}

void Tutorial_HUD::Start()
{
	// Attack
	X			= game_object->GetChild("Attack")->GetChild("AttackX")->GetComponent<ComponentImage>();
	Y			= game_object->GetChild("Attack")->GetChild("AttackY")->GetComponent<ComponentImage>();
	text_attack = game_object->GetChild("Attack")->GetChild("Text")->GetComponent<ComponentText>();
	X->SetBackgroundColor(1, 1, 1, 0);
	Y->SetBackgroundColor(1, 1, 1, 0);
	text_attack->SetAlpha(0);

	// Dash
	RB = game_object->GetChild("Dash")->GetChild("RB")->GetComponent<ComponentImage>();
	text_dash = game_object->GetChild("Dash")->GetChild("Text")->GetComponent<ComponentText>();
	RB->SetBackgroundColor(1, 1, 1, 0);
	text_dash->SetAlpha(0);

	// Magic
	magic_images.push_back(game_object->GetChild("Magic")->GetChild("X")->GetComponent<ComponentImage>());
	magic_images.push_back(game_object->GetChild("Magic")->GetChild("Y")->GetComponent<ComponentImage>());
	magic_images.push_back(game_object->GetChild("Magic")->GetChild("A")->GetComponent<ComponentImage>());
	magic_images.push_back(game_object->GetChild("Magic")->GetChild("B")->GetComponent<ComponentImage>());
	magic_images.push_back(game_object->GetChild("Magic")->GetChild("LT")->GetComponent<ComponentImage>());
	text_magic = game_object->GetChild("Magic")->GetChild("Text")->GetComponent<ComponentText>();
	text_magic->SetAlpha(0);

	auto iter = magic_images.begin();
	for (; iter != magic_images.end(); ++iter)
	{
		(*iter)->SetBackgroundColor(1, 1, 1, 0);
	}
}

void Tutorial_HUD::Update()
{
	HandleTriggerAttack();

	HandleTriggerDash();

	HandleTriggerMagic();
}

void Tutorial_HUD::HandleTriggerDash()
{
	switch (current_state_dash)
	{
	case Current_Showing::FADING_IN:
	{
		float t = (Time::GetTimeSinceStart() - dash_time) / 0.5f;
		float lerp = Maths::Lerp(0.0f, 1.0f, t);

		RB->SetBackgroundColor(1, 1, 1, lerp);
		text_dash->SetAlpha(lerp);

		if (t >= 1)
		{
			RB->SetBackgroundColor(1, 1, 1, 1);
			text_dash->SetAlpha(1);

			current_state_dash = Current_Showing::SHOWING;
		}

		break;
	}
	case Current_Showing::SHOWING:
	{
		break;
	}
	case Current_Showing::FADING_OUT:
	{
		float t = (Time::GetTimeSinceStart() - dash_time) / 0.5f;
		float lerp = Maths::Lerp(1.0f, 0.0f, t);

		RB->SetBackgroundColor(1, 1, 1, lerp);
		text_dash->SetAlpha(lerp);

		if (t >= 1)
		{
			RB->SetBackgroundColor(1, 1, 1, 0);
			text_dash->SetAlpha(0);

			current_state_dash = Current_Showing::ANY;
		}

		break;
	}
	case Current_Showing::ANY:
	{
		break;
	}
	}
}

void Tutorial_HUD::HandleTriggerMagic()
{
	switch (current_state_magic)
	{
	case Current_Showing::FADING_IN:
	{
		float t = (Time::GetTimeSinceStart() - magic_time) / 0.5f;
		float lerp = Maths::Lerp(0.0f, 1.0f, t);

		auto iter = magic_images.begin();
		for (; iter != magic_images.end(); ++iter)
		{
			(*iter)->SetBackgroundColor(1, 1, 1, lerp);
		}
		text_magic->SetAlpha(lerp);

		if (t >= 1)
		{
			auto iter = magic_images.begin();
			for (; iter != magic_images.end(); ++iter)
			{
				(*iter)->SetBackgroundColor(1, 1, 1, 1);
			}
			text_magic->SetAlpha(1);

			current_state_magic = Current_Showing::SHOWING;
		}

		break;
	}
	case Current_Showing::SHOWING:
	{
		break;
	}
	case Current_Showing::FADING_OUT:
	{
		float t = (Time::GetTimeSinceStart() - magic_time) / 0.5f;
		float lerp = Maths::Lerp(1.0f, 0.0f, t);

		auto iter = magic_images.begin();
		for (; iter != magic_images.end(); ++iter)
		{
			(*iter)->SetBackgroundColor(1, 1, 1, lerp);
		}
		text_magic->SetAlpha(lerp);


		if (t >= 1)
		{
			auto iter = magic_images.begin();
			for (; iter != magic_images.end(); ++iter)
			{
				(*iter)->SetBackgroundColor(1, 1, 1, 0);
			}
			text_magic->SetAlpha(0);


			current_state_magic = Current_Showing::ANY;
		}

		break;
	}
	case Current_Showing::ANY:
	{
		break;
	}
	}
}

void Tutorial_HUD::HandleTriggerAttack()
{
	switch (current_state_attack)
	{
	case Current_Showing::FADING_IN:
	{
		float t = (Time::GetTimeSinceStart() - attack_time) / 0.5f;
		float lerp = Maths::Lerp(0.0f, 1.0f, t);

		X->SetBackgroundColor(1, 1, 1, lerp);
		Y->SetBackgroundColor(1, 1, 1, lerp);
		text_attack->SetAlpha(lerp);

		if (t >= 1)
		{
			X->SetBackgroundColor(1, 1, 1, 1);
			Y->SetBackgroundColor(1, 1, 1, 1);
			text_attack->SetAlpha(1);

			current_state_attack = Current_Showing::SHOWING;
		}

		break;
	}
	case Current_Showing::SHOWING:
	{
		break;
	}
	case Current_Showing::FADING_OUT:
	{
		float t = (Time::GetTimeSinceStart() - attack_time) / 0.5f;
		float lerp = Maths::Lerp(1.0f, 0.0f, t);

		X->SetBackgroundColor(1, 1, 1, lerp);
		Y->SetBackgroundColor(1, 1, 1, lerp);
		text_attack->SetAlpha(lerp);

		if (t >= 1)
		{
			X->SetBackgroundColor(1, 1, 1, 0);
			Y->SetBackgroundColor(1, 1, 1, 0);
			text_attack->SetAlpha(0);

			current_state_attack = Current_Showing::ANY;
		}

		break;
	}
	case Current_Showing::ANY:
	{
		break;
	}
	}
}
