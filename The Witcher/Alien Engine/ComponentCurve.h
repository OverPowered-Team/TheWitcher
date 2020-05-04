#pragma once

#include "Component.h"
#include "MathGeoLib/include/Math/float3.h"
#include <vector>

class __declspec(dllexport) Curve {
public:
	Curve() {}
	Curve(const float3& begin, const float3& end, const float3& position);
	~Curve() {}

	// 0 - 1
	float3 ValueAt(float at);
	float3 NormalAt(float at);

	const std::vector<float3>& GetControlPoints();
	const std::vector<float3>& GetControlPointsNormals();

	void SetControlPointAt(int index, const float3& value);
	void SetControlPointNormalAt(int index, const float3& value);

	void SetDetail(int detail);

	void AddSegment(bool begin);

	void InsertControlPoint(int index);
	void RemoveControlPoint(int index);

	void UpdatePosition(const float3& new_position);

	void SetPoints(const std::vector<float3>& controlPoints, const std::vector<float3>& normalPoints);

private:

	void Refresh();

	void CalculateCurvePoints();
	float3 CalculateBezier(float t, const float3& p0, const float3& p1, const float3& p2, const float3& p3);

public:

	int detail = 50;

private:

	std::vector<float3> control_points;
	std::vector<float3> control_points_normals;
	std::vector<float3> curve_normals;
	std::vector<float3> curve_points;

	float3 position = float3::zero();
};

class __declspec(dllexport) ComponentCurve : public Component {
	friend class GameObject;
public:

	ComponentCurve(GameObject* attach);
	virtual ~ComponentCurve();

	void UpdatePosition(const float3& new_position);

private:

	void DrawScene();

	bool DrawInspector();

	void SaveComponent(JSONArraypack* to_save);
	void LoadComponent(JSONArraypack* to_load);

public:

	Curve curve;

	bool renderTensors = true;
	bool renderNormals = true;
	bool renderOnSelected = false;
};