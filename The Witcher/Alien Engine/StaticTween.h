#pragma once

#include "GameObject.h"
#include "MathGeoLib/include/Math/float3.h"

enum TweenType;

class __declspec(dllexport) Tween {
public:

	enum {
		notUsed = -1, linear, easeIn, easeOut, easeInOut, easeInBack, easeOutBack, easeInOutBack, easeInBounce, easeOutBounce, easeInOutBounce,
		easeInElastic, easeOutElastic, easeInOutElastic
	};

public:
	static void TweenMove(GameObject* gameObject, const float3& to, float time, int type = 0);
	static void TweenMoveTo(GameObject* gameObject, const float3& to, float time, int type = 0);
	static void TweenScale(GameObject* gameObject, const float3& to, float time, int type = 0);
	static void TweenRotate(GameObject* gameObject, const float3& to, float time, int type = 0);
	// Color values has to go from 0 to 1 
	static void TweenColor(GameObject* gameObject, const float3& to, float time, int type = 0);
	// Alpha values has to go from 0 to 1 
	static void TweenAlpha(GameObject* gameObject, const float to, float time, int type = 0);
	// Color and alpha values has to go from 0 to 1 
	static void TweenRGBA(GameObject* gameObject, const float4& to, float time, int type = 0);

	// Color values has to go from 0 to 1 
	static void TweenUIColor(GameObject* gameObject, const float3& to, float time, int type = 0);
	// Alpha values has to go from 0 to 1 
	static void TweenUIAlpha(GameObject* gameObject, const float to, float time, int type = 0);
	// Color and alpha values has to go from 0 to 1 
	static void TweenUIRGBA(GameObject* gameObject, const float4& to, float time, int type = 0);

};