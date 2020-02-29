#pragma once

#include "MathGeoLib/include/Math/quat.h"
#include "MathGeoLib/include/Math/float3.h"
#include "ComponentCamera.h"

enum class BillboardType
{
	SCREEN = 0,
	WORLD,
	AXIS,

	// ----

	NONE
};

namespace Billboard
{

	Quat AlignToScreen(ComponentCamera* camera);

	Quat AlignToWorld(ComponentCamera* camera, float3 position);

	Quat AlignToAxis(ComponentCamera* camera, float3 position, float3 axis = float3::zero());

}


