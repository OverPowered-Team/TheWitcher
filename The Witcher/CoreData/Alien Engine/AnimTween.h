#pragma once

#include "Module.h"

class ModuleTween;
class GameObject;

#define MAX_TWEENS 20

enum TweenAction {
	NONE = -1,
	MOVE_TO,
	MOVE,
	ROTATE,
	SCALE,
	ALPHA,
	COLOR,
	RGBA,
	UI_COLOR,
	UI_ALPHA,
	UI_RGBA
};

enum TweenType {
	notUsed = -1, linear, easeIn, easeOut, easeInOut, easeInBack, easeOutBack, easeInOutBack, easeInBounce, easeOutBounce, easeInOutBounce,
	easeInElastic, easeOutElastic, easeInOutElastic
};

class AnimTween: public Module
{
public:
	AnimTween() {};
	~AnimTween() {};

	update_status Update(float dt);
	void HandleEvent(EventType eventType);

public:
	ModuleTween* CreateTween(GameObject* gameObject, float time, TweenAction action, TweenType type);
	ModuleTween* TweenMove(GameObject* gameObject, const float3& to, float time, TweenType type);
	ModuleTween* TweenMoveTo(GameObject* gameObject, const float3& to, float time, TweenType type);
	ModuleTween* TweenScale(GameObject* gameObject, const float3& to, float time, TweenType type);
	ModuleTween* TweenRotate(GameObject* gameObject, const float3& to, float time, TweenType type);
	ModuleTween* TweenColor(GameObject* gameObject, const float3& to, float time, TweenType type);
	ModuleTween* TweenAlpha(GameObject* gameObject, const float to, float time, TweenType type);
	ModuleTween* TweenRGBA(GameObject* gameObject, const float4& to, float time, TweenType type);

	ModuleTween* TweenUIColor(GameObject* gameObject, const float3& to, float time, TweenType type);
	ModuleTween* TweenUIAlpha(GameObject* gameObject, const float to, float time, TweenType type);
	ModuleTween* TweenUIRGBA(GameObject* gameObject, const float4& to, float time, TweenType type);

private:
	std::vector<ModuleTween*> tweens;
};
