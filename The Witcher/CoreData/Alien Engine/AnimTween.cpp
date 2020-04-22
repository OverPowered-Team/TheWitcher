#include "AnimTween.h"
#include "ModuleTween.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ResourceMaterial.h"
#include "ComponentUI.h"
#include "Event.h"

#include "mmgr/mmgr.h"

update_status AnimTween::Update(float dt)
{
	for (auto it_tween = tweens.begin(); it_tween != tweens.end();)
	{
		if (!(*it_tween)->UpdateInternal())
		{
			delete (*it_tween);
			it_tween = tweens.erase(it_tween);
		}
		else
			it_tween++;
	}
	return UPDATE_CONTINUE;
}

void AnimTween::HandleEvent(EventType eventType)
{
	switch (eventType)
	{
	case EventType::ON_UNLOAD_SCENE:
		for (auto it_tween = tweens.begin(); it_tween != tweens.end();) {
			delete *it_tween;
			it_tween = tweens.erase(it_tween);
		}
		break;
	}
}

ModuleTween* AnimTween::CreateTween(GameObject* gameObject, float time, TweenAction action, TweenType type)
{
	if (tweens.size() > MAX_TWEENS)
	{
		LOG_ENGINE("Max tweens %i has been reached. :(", MAX_TWEENS);
		return nullptr;
	}
		
	ModuleTween* tween = new ModuleTween();
	tween->trans = gameObject->transform;
	tween->time = time;
	tween->action = action;
	tween->type = type;

	tweens.push_back(tween);
	return tween;
}

ModuleTween* AnimTween::TweenMove(GameObject* gameObject, const float3& to, float time, TweenType type)
{
	ModuleTween* tween = CreateTween(gameObject, time, TweenAction::MOVE, type);
	if (tween)
	{
		tween->from = float4(gameObject->transform->GetGlobalPosition(), 0.0f);
		tween->to = tween->from + float4(to, 0.f);
	}

	return tween;
}

ModuleTween* AnimTween::TweenMoveTo(GameObject* gameObject, const float3& to, float time, TweenType type)
{
	ModuleTween* tween = CreateTween(gameObject, time, TweenAction::MOVE_TO, type);
	if (tween)
	{
		tween->from = float4(gameObject->transform->GetGlobalPosition(), 0.0f);
		tween->to = float4(to, 0.f);
	}

	return tween;
}

ModuleTween* AnimTween::TweenScale(GameObject* gameObject, const float3& to, float time, TweenType type)
{
	ModuleTween* tween = CreateTween(gameObject, time, TweenAction::SCALE, type);
	if (tween)
	{
		tween->from = float4(gameObject->transform->GetGlobalScale(), 0.0f);
		tween->to = float4(to, 0.f);
	}

	return tween;
}

ModuleTween* AnimTween::TweenRotate(GameObject* gameObject, const float3& to, float time, TweenType type)
{
	ModuleTween* tween = CreateTween(gameObject, time, TweenAction::ROTATE, type);
	if (tween)
	{
		Quat rot = gameObject->transform->GetGlobalRotation();
		tween->from = float4(rot.x, rot.y, rot.x, rot.w);

		Quat end_rot = Quat::FromEulerXYZ(DegToRad(to.x), DegToRad(to.y), DegToRad(to.z));
		tween->to = float4(end_rot.x, end_rot.y, end_rot.z, end_rot.w);
	}

	return tween;
}

ModuleTween* AnimTween::TweenColor(GameObject* gameObject, const float3& to, float time, TweenType type)
{
	ComponentMaterial* material = gameObject->GetComponent<ComponentMaterial>();
	if (!material)
	{
		LOG_ENGINE("There's no material in the gameobject!");
		return nullptr;
	}

	ModuleTween* tween = CreateTween(gameObject, time, TweenAction::COLOR, type);
	if(tween)
	{
		tween->from = float4(material->material->color.x, material->material->color.y, material->material->color.z, material->material->color.w);
		tween->to = float4(to, 0.f);
	}

	return tween;
}

ModuleTween* AnimTween::TweenAlpha(GameObject* gameObject, const float to, float time, TweenType type)
{
	ComponentMaterial* material = gameObject->GetComponent<ComponentMaterial>();
	if (!material)
	{
		LOG_ENGINE("There's no material in the gameobject!");
		return nullptr;
	}

	ModuleTween* tween = CreateTween(gameObject, time, TweenAction::ALPHA, type);
	if (tween)
	{
		tween->from = float4(material->material->color.x, material->material->color.y, material->material->color.z, material->material->color.w);
		tween->to = float4(material->material->color.x, material->material->color.y, material->material->color.z, to);
	}

	return tween;
}

ModuleTween* AnimTween::TweenRGBA(GameObject* gameObject, const float4& to, float time, TweenType type)
{
	ComponentMaterial* material = gameObject->GetComponent<ComponentMaterial>();
	if (!material)
	{
		LOG_ENGINE("There's no material in the gameobject!");
		return nullptr;
	}

	ModuleTween* tween = CreateTween(gameObject, time, TweenAction::RGBA, type);
	if (tween)
	{
		tween->from = float4(material->material->color.x, material->material->color.y, material->material->color.z, material->material->color.w);
		tween->to = to;
	}

	return tween;
}

ModuleTween* AnimTween::TweenUIColor(GameObject* gameObject, const float3& to, float time, TweenType type)
{
	ComponentUI* ui = gameObject->GetComponent<ComponentUI>();
	if (!ui)
	{
		LOG_ENGINE("There's no component UI in the gameobject!");
		return nullptr;
	}

	ModuleTween* tween = CreateTween(gameObject, time, TweenAction::UI_COLOR, type);
	if (tween)
	{
		tween->from = float4(ui->current_color.r, ui->current_color.g, ui->current_color.b, ui->current_color.a);
		tween->to = float4(to, 0.f);
	}

	return tween;
}

ModuleTween* AnimTween::TweenUIAlpha(GameObject* gameObject, const float to, float time, TweenType type)
{
	ComponentUI* ui = gameObject->GetComponent<ComponentUI>();
	if (!ui)
	{
		LOG_ENGINE("There's no component UI in the gameobject!");
		return nullptr;
	}

	ModuleTween* tween = CreateTween(gameObject, time, TweenAction::UI_ALPHA, type);
	if (tween)
	{
		tween->from = float4(ui->current_color.r, ui->current_color.g, ui->current_color.b, ui->current_color.a);
		tween->to = float4(ui->current_color.r, ui->current_color.g, ui->current_color.b, to);
	}

	return tween;
}

ModuleTween* AnimTween::TweenUIRGBA(GameObject* gameObject, const float4& to, float time, TweenType type)
{
	ComponentUI* ui = gameObject->GetComponent<ComponentUI>();
	if (!ui)
	{
		LOG_ENGINE("There's no component UI in the gameobject!");
		return nullptr;
	}

	ModuleTween* tween = CreateTween(gameObject, time, TweenAction::UI_RGBA, type);
	if (tween)
	{
		tween->from = float4(ui->current_color.r, ui->current_color.g, ui->current_color.b, ui->current_color.a);
		tween->to = to;
	}

	return tween;
}

