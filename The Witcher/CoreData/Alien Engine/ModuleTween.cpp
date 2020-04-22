#include "ModuleTween.h"
#include "Maths.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ResourceMaterial.h"
#include "ComponentUI.h"
#include "Time.h"

#include "mmgr/mmgr.h"

bool ModuleTween::UpdateInternal()
{
	bool ret = true;
	float4 current_value = float4::zero;
	current_value = CalculateInternal();

	current_time += Time::GetDT();

	ComponentMaterial* material = nullptr;
	ComponentUI* ui = nullptr;

	switch(action)
	{
	case TweenAction::MOVE:
		trans->SetGlobalPosition(float3(current_value.x, current_value.y, current_value.z));
		break;
	case TweenAction::MOVE_TO:
		trans->SetGlobalPosition(float3(current_value.x, current_value.y, current_value.z));
		break;
	case TweenAction::SCALE:
		//trans->SetGlobalScale(float3(current_scale.x, current_scale.y, current_scale.z));
		break;
	case TweenAction::ROTATE:
		//trans->SetGlobalRotation(Quat(current_value.x, current_value.y, current_value.z, current_value.w));
		break;
	case TweenAction::COLOR:
		material = trans->game_object_attached->GetComponent<ComponentMaterial>();
		if (material)
			material->material->color = { current_value.x, current_value.y, current_value.z, material->material->color.w };
		break;
	case TweenAction::ALPHA:
		material = trans->game_object_attached->GetComponent<ComponentMaterial>();
		if (material)
			material->material->color.w = current_value.w;
		break;
	case TweenAction::RGBA:
		material = trans->game_object_attached->GetComponent<ComponentMaterial>();
		if (material)
			material->material->color = { current_value.x, current_value.y, current_value.z, current_value.w };
		break;
	case TweenAction::UI_COLOR:
		ui = trans->game_object_attached->GetComponent<ComponentUI>();
		if (ui)
			ui->current_color = { current_value.x, current_value.y, current_value.z, ui->current_color.a };
		break;
	case TweenAction::UI_ALPHA:
		ui = trans->game_object_attached->GetComponent<ComponentUI>();
		if (ui)
			ui->current_color = { ui->current_color.r, ui->current_color.g, ui->current_color.b, current_value.w };
		break;
	case TweenAction::UI_RGBA:
		ui = trans->game_object_attached->GetComponent<ComponentUI>();
		if (ui)
			ui->current_color = { current_value.x, current_value.y, current_value.z, current_value.w };
		break;
	}

	if (current_time >= time)
		ret = false;

	return ret;
}

float4 ModuleTween::CalculateInternal()
{
	float4 current_value = float4::zero;

	switch (type)
	{
	case notUsed:
		break;
	case linear:
		current_value = LinealLerp();
		break;
	case easeIn:
		current_value = EaseInQuadratic();
		break;
	case easeOut:
		current_value = EaseOutQuadratic();
		break;
	case easeInOut:
		current_value = EaseInOutQuadratic();
		break;
	case easeInElastic:
		current_value = EaseInElastic();
		break;
	case easeOutElastic:
		current_value = EaseOutElastic();
		break;
	case easeOutBounce:
		current_value = EaseOutBounce();
		break;
	case easeInBack:
		current_value = EaseInBack();
		break;
	case easeOutBack:
		current_value = EaseOutBack();
		break;
	default:
		break;
	}

	return current_value;
}

float4 ModuleTween::LinealLerp()
{
	float weight = current_time / time;
	return math::Lerp(from, to, weight);
}

float4 ModuleTween::EaseInQuadratic()
{
	float aux_time = current_time / time;
	return (to - from) * aux_time * aux_time + from;
}

float4 ModuleTween::EaseOutQuadratic()
{
	float aux_time = current_time / time;
	return -(to - from) * aux_time * (aux_time - 2) + from;
}

float4 ModuleTween::EaseInOutQuadratic()
{
	float aux_time = current_time / (time * 0.5);
	if (aux_time < 1) 
		return (to - from) / 2 * aux_time * aux_time + from;
	aux_time--;
	return -(to - from) / 2 * (aux_time * (aux_time - 2) - 1) + from;
}

float4 ModuleTween::EaseOutElastic()
{
	float aux_time = current_time / time;
	if (aux_time == 1)
		return from + to;

	float p = time * .3f;
	float s = p / 4;

	return ((to - from) * math::Pow(2, -10 * aux_time) * math::Sin((aux_time * current_time - s) * (2 * math::pi) / p) + (to - from) + from);
}

float4 ModuleTween::EaseInElastic()
{
	float aux_time = current_time / time;
	if (aux_time == 1)
		return from + to;

	float p = time * .3f;
	float s = p / 4;

	return -((to - from) * math::Pow(2, 10 * (aux_time -= 1)) * math::Sin((aux_time * time - s) * (2 * math::pi) / p)) + from;
}

float4 ModuleTween::EaseOutBounce()
{
	float aux_time = current_time / time;

	if (aux_time < (1.f / 2.75f))
		return to * (7.5625f * aux_time * aux_time) + from;
	else if (aux_time < (2.f / 2.75f))
		return to * (7.5625f * (aux_time -= (1.5f / 2.75f)) * aux_time + .75f) + from;
	else if (aux_time < (2.5f / 2.75f))
		return to * (7.5625f * (aux_time -= (2.25f / 2.75f)) * aux_time + .9375f) + from;
	else
		return to * (7.5625f * (aux_time -= (2.625f / 2.75f)) * aux_time + .984375f) + from;
}

float4 ModuleTween::EaseOutBack()
{
	float aux_time = current_time / time - 1;
	return (to - from) * (aux_time * aux_time * ((1.70158f + 1) * aux_time + 1.70158f) + 1) + from;
}

float4 ModuleTween::EaseInBack()
{
	float aux_time = current_time / time;
	return (to - from) * aux_time * aux_time * ((1.70158f + 1) * aux_time - 1.70158f) + from;
}
