#pragma once

#include "AnimTween.h"
#include "MathGeoLib/include/Math/float4.h"

class ComponentTransform;

class ModuleTween {

public:
	bool UpdateInternal();

private:
	float4 CalculateInternal();

	float4 LinealLerp();

	float4 EaseInQuadratic();
	float4 EaseOutQuadratic();
	float4 EaseInOutQuadratic();

	float4 EaseOutElastic();
	float4 EaseInElastic();

	float4 EaseOutBounce();

	float4 EaseOutBack();

	float4 EaseInBack();

public:
	float4 from = { 0.0f,0.0f ,0.0f ,0.0f };
	float4 to = { 0.0f,0.0f ,0.0f ,0.0f };
	float time = 0.F;
	float current_time = 0.F;
	TweenAction action = TweenAction::NONE;
	TweenType type = TweenType::notUsed;
	ComponentTransform* trans = nullptr;

	float delay = 0.F;
};
