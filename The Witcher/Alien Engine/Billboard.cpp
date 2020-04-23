#include "Billboard.h"

Quat Billboard::AlignToScreen(ComponentCamera* camera)
{
	float4x4 viewMatrix = camera->GetViewMatrix4x4();
	float3x3 rot = float3x3(viewMatrix.WorldX(), viewMatrix.WorldY(), viewMatrix.WorldZ());
	return rot.ToQuat();
}

Quat Billboard::AlignToWorld(ComponentCamera* camera, float3 position)
{
	//float3 front = (camera->GetPos() - transform->GetBoundingBox().CenterPoint()).Normalized(); // The object may not have BoundingBox (?)
	float3 front = (camera->GetCameraPosition() - position).Normalized();
	float3 up = camera->GetViewMatrix4x4().WorldY();
	float3 right = up.Cross(front);
	up = front.Cross(right);

	float3x3 rot = float3x3(right, up, front);
	return rot.ToQuat();
}

Quat Billboard::AlignToAxis(ComponentCamera* camera, float3 position, float3 axis)
{
	// TODO:: Maybe find another way to handle this && take other axis into account
	float3 front = (camera->GetCameraPosition() - position).Normalized();
	float3 up = float3::unitY();
	float3 right = up.Cross(front);
	front = right.Cross(up);

	float3x3 rot = float3x3(right, up, front);
	return rot.ToQuat();
}

Quat Billboard::AlignToVelocity(ComponentCamera* camera, float3 position, float3 velocity)
{
	float3 vz = (camera->GetCameraPosition() - position).Normalized();
	vz.Normalized();

	velocity.Normalized();
	float3 vx = velocity.Cross(vz);
	vx.Normalized();

	float3 vy = vz.Cross(vx);

	//float4x4 rot2 = float4x4(float4(vy, 0.0f), float4(vx, 0.0f), float4(vz, 0.0f), float4(position, 1.0f));
	float3x3 rot = float3x3(vx, vy, vz);
	
	return rot.ToQuat();

	//float4x4 viewMatrix = camera->GetViewMatrix4x4();
	//float3 z = (camera->GetCameraPosition() - position).Normalized();
	//
	////float3 z = viewMatrix.MulDir(front).Normalized();

	//float3 y = velocity.Normalized();

	//float3 x = y.Cross(z);

	//y = z.Cross(x);

	//float3x3 rot = float3x3(x, y, z);
	//return rot.ToQuat();


}

