#include "StaticTween.h"
#include "Application.h"
#include "ModuleTween.h"

void Tween::TweenMove(GameObject* gameObject, const float3& to, float time, int type)
{
	App->tween->TweenMove(gameObject, to, time, (TweenType)type);
}

void Tween::TweenMoveTo(GameObject* gameObject, const float3& to, float time, int type)
{
	App->tween->TweenMoveTo(gameObject, to, time, (TweenType)type);
}

void Tween::TweenScale(GameObject* gameObject, const float3& to, float time, int type)
{
	App->tween->TweenScale(gameObject, to, time, (TweenType)type);
}

void Tween::TweenRotate(GameObject* gameObject, const float3& to, float time, int type)
{
	App->tween->TweenRotate(gameObject, to, time, (TweenType)type);
}

void Tween::TweenColor(GameObject* gameObject, const float3& to, float time, int type)
{
	App->tween->TweenColor(gameObject, to, time, (TweenType)type);
}

void Tween::TweenAlpha(GameObject* gameObject, const float to, float time, int type)
{
	App->tween->TweenAlpha(gameObject, to, time, (TweenType)type);
}

void Tween::TweenRGBA(GameObject* gameObject, const float4& to, float time, int type)
{
	App->tween->TweenRGBA(gameObject, to, time, (TweenType)type);
}

void Tween::TweenUIColor(GameObject* gameObject, const float3& to, float time, int type)
{
	App->tween->TweenUIColor(gameObject, to, time, (TweenType)type);
}

void Tween::TweenUIAlpha(GameObject* gameObject, const float to, float time, int type)
{
	App->tween->TweenUIAlpha(gameObject, to, time, (TweenType)type);;
}

void Tween::TweenUIRGBA(GameObject* gameObject, const float4& to, float time, int type)
{
	App->tween->TweenUIRGBA(gameObject, to, time, (TweenType)type);;
}
