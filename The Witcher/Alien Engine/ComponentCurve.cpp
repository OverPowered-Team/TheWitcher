#include "ComponentCurve.h"
#include "imgui/imgui.h"
#include "ReturnZ.h"
#include "Maths.h"
#include "glew/include/glew.h"
#include "imgui/imgui_internal.h"
#include "Application.h"
#include "ModuleUI.h"
#include "PanelScene.h"
#include "ComponentTransform.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleObjects.h"

ComponentCurve::ComponentCurve(GameObject* attach) : Component(attach)
{
	type = ComponentType::CURVE;

	curve = Curve(float3{ -10.f, 0.f, 0.f }, float3{ 10.f, 0.f, 0.f }, game_object_attached->transform->GetGlobalPosition());

#ifndef GAME_VERSION
	App->objects->debug_draw_list.emplace(this, std::bind(&ComponentCurve::DrawScene, this));
#endif
}

ComponentCurve::~ComponentCurve()
{
#ifndef GAME_VERSION
	App->objects->debug_draw_list.erase(App->objects->debug_draw_list.find(this));
#endif
}

void ComponentCurve::UpdatePosition(const float3& new_position)
{
	curve.UpdatePosition(new_position);
}

bool ComponentCurve::DrawInspector()
{
	static bool en;
	ImGui::PushID(this);
	en = enabled;
	if (ImGui::Checkbox("##CmpActive", &en)) {
		ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
		enabled = en;
		if (!enabled)
			OnDisable();
		else
			OnEnable();
	}

	ImGui::PopID();
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Curve", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		RightClickMenu("Curve");
		ImGui::Spacing();

		ImGui::Checkbox("Render On Selected", &renderOnSelected);
		ImGui::SameLine();
		ImGui::Checkbox("Render Normals", &renderNormals);
		ImGui::SameLine();
		ImGui::Checkbox("Render Tensors", &renderTensors);

		ImGui::Spacing();
		if (ImGui::DragInt("Detail", &curve.detail)) {
			curve.SetDetail(curve.detail);
		}
		ImGui::Spacing();

		if (ImGui::Button("Add Segment in the beggining")) {
			curve.AddSegment(true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Add Segment in the end")) {
			curve.AddSegment(false);
		}
		
		ImGui::Spacing();
		if (ImGui::TreeNodeEx("Segments", ImGuiTreeNodeFlags_SpanAvailWidth)) {
			for (uint i = 0; i < curve.GetControlPoints().size() - 1; i += 3) {
				if (ImGui::TreeNodeEx(std::string("Segment " + std::to_string(int(i / (float)3) + 1)).data(), ImGuiTreeNodeFlags_SpanAvailWidth)) {
					ImGui::Spacing();
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
					ImGui::Text("Start Point");
					ImGui::SameLine(150);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
					ImGui::PushID(323123 + i);
					if (ImGui::DragFloat3("##FF", (float*)curve.GetControlPoints()[i].ptr())) {
						curve.SetControlPointAt(i, curve.GetControlPoints()[i]);
					}
					ImGui::PopID();

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
					ImGui::Text("End Point");
					ImGui::SameLine(150);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
					ImGui::PushID(323123 + i + 3);
					if (ImGui::DragFloat3("##FF", (float*)curve.GetControlPoints()[i + 3].ptr())) {
						curve.SetControlPointAt(i + 3, curve.GetControlPoints()[i + 3]);
					}
					ImGui::PopID();

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
					ImGui::Text("Tensor 1");
					ImGui::SameLine(150);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
					ImGui::PushID(323123 + i + 1);
					if (ImGui::DragFloat3("##FF", (float*)curve.GetControlPoints()[i + 1].ptr())) {
						curve.SetControlPointAt(i + 1, curve.GetControlPoints()[i + 1]);
					}
					ImGui::PopID();

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
					ImGui::Text("Tensor 2");
					ImGui::SameLine(150);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
					ImGui::PushID(323123 + i + 2);
					if (ImGui::DragFloat3("##FF", (float*)curve.GetControlPoints()[i + 2].ptr())) {
						curve.SetControlPointAt(i + 2, curve.GetControlPoints()[i + 2]);
					}
					ImGui::PopID();

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
					ImGui::Text("Normals Start");
					ImGui::SameLine(150);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
					ImGui::PushID(8767 + i + 2);
					if (ImGui::DragFloat3("##FF", (float*)curve.GetControlPointsNormals()[i / 3].ptr(), 0.1F, -1, 1)) {
						curve.SetControlPointNormalAt(i / 3, curve.GetControlPointsNormals()[i / 3]);
					}
					ImGui::PopID();

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
					ImGui::Text("Normals End");
					ImGui::SameLine(150);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
					ImGui::PushID(6665 + i + 2);
					if (ImGui::DragFloat3("##FF", (float*)curve.GetControlPointsNormals()[(i / 3) + 1].ptr(), 0.1F, -1, 1)) {
						curve.SetControlPointNormalAt((i / 3) + 1, curve.GetControlPointsNormals()[(i / 3) + 1]);
					}
					ImGui::PopID();

					float width = ImGui::GetItemRectSize().x - 8;
					ImGui::Spacing();

					if (curve.GetControlPoints().size() == 4) {
						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
					}

					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 101);
					ImGui::PushID(989 + i);
					if (ImGui::Button("Remove Begin", { width * 0.5F, 0 })) {
						App->ui->panel_scene->gizmo_curve = false;
						App->ui->panel_scene->curve = nullptr;
						App->ui->panel_scene->curve_index = 0;
						curve.RemoveControlPoint(i);
						ImGui::PopID();
						ImGui::TreePop();
						break;
					}
					ImGui::PopID();
					ImGui::SameLine();
					ImGui::PushID(64343 + i);
					if (ImGui::Button("Remove End", { width * 0.5F, 0 })) {
						App->ui->panel_scene->gizmo_curve = false;
						App->ui->panel_scene->curve = nullptr;
						App->ui->panel_scene->curve_index = 0;
						curve.RemoveControlPoint(i + 3);
						ImGui::PopID();
						ImGui::TreePop();
						break;
					}
					ImGui::PopID();


					if (curve.GetControlPoints().size() == 4) {
						ImGui::PopItemFlag();
						ImGui::PopStyleVar();
					}
					ImGui::Spacing();
					ImGui::PushID(32 + i);
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 101);
					if (ImGui::Button("Add Control Point", { width + 8, 0 })) {
						App->ui->panel_scene->gizmo_curve = false;
						App->ui->panel_scene->curve = nullptr;
						App->ui->panel_scene->curve_index = 0;
						curve.InsertControlPoint(i);
					}
					ImGui::PopID();


					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}
	else {
		RightClickMenu("Curve");
	}

	return true;
}

void ComponentCurve::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetBoolean("Enabled", enabled);
	to_save->SetNumber("Type", (int)type);
	to_save->SetBoolean("Tensors", renderTensors);
	to_save->SetBoolean("Normals", renderNormals);
	to_save->SetBoolean("OnSelected", renderOnSelected);
	to_save->SetNumber("Detail", curve.detail);

	JSONArraypack* controlPoints = to_save->InitNewArray("ControlPoints");
	for (uint i = 0; i < curve.GetControlPoints().size(); ++i) {
		controlPoints->SetAnotherNode();
		controlPoints->SetFloat3("ControlPoint", curve.GetControlPoints()[i]);
	}

	JSONArraypack* normals = to_save->InitNewArray("NormalsPoints");
	for (uint i = 0; i < curve.GetControlPointsNormals().size(); ++i) {
		normals->SetAnotherNode();
		normals->SetFloat3("NormalPoint", curve.GetControlPointsNormals()[i]);
	}
}

void ComponentCurve::LoadComponent(JSONArraypack* to_load)
{
	enabled = to_load->GetBoolean("Enabled");
	renderNormals = to_load->GetBoolean("Normals");
	renderTensors = to_load->GetBoolean("Tensors");
	renderOnSelected = to_load->GetBoolean("OnSelected");

	curve.detail = to_load->GetNumber("Detail");

	std::vector<float3> control_points;
	std::vector<float3> control_points_normals;

	JSONArraypack* controlPoints = to_load->GetArray("ControlPoints");
	for (uint i = 0; i < controlPoints->GetArraySize(); ++i) {
		control_points.push_back(controlPoints->GetFloat3("ControlPoint"));
		controlPoints->GetAnotherNode();
	}

	JSONArraypack* normalPoints = to_load->GetArray("NormalsPoints");
	for (uint i = 0; i < normalPoints->GetArraySize(); ++i) {
		control_points_normals.push_back(normalPoints->GetFloat3("NormalPoint"));
		normalPoints->GetAnotherNode();
	}

	curve.SetPoints(control_points, control_points_normals);
}

void ComponentCurve::DrawScene()
{
	if (!game_object_attached->IsSelected() && renderOnSelected) {
		return;
	}

	glDisable(GL_LIGHTING);

	// Draw Control Points
	glPointSize(9);
	glBegin(GL_POINTS);

	auto control_points = curve.GetControlPoints();
	for (uint i = 0; i < control_points.size(); ++i) {
		if (i % 3 == 0) {
			glColor3f(0.f, 1.f, 0.f);
		}
		else {
			glColor3f(1.f, 0.f, 0.f);
		}
		glVertex3f(control_points[i].x, control_points[i].y, control_points[i].z);
	}
	glEnd();
	glPointSize(1);

	// Draw Curve
	glColor3f(0, 0, 1);
	glBegin(GL_LINE_STRIP);
	for (int f = 0; f <= curve.detail; ++f) {
		float3 res = curve.ValueAt(f/(float)curve.detail);
		glVertex3f(res[0], res[1], res[2]);
	}
	glEnd();
	
	if (renderTensors) {
		// Draw Tensors
		glColor3f(1.f, 1.f, 0.f);
		for (uint i = 0; i < control_points.size(); i += 3) {
			glBegin(GL_LINE_STRIP);
			if (i == 0) {
				glVertex3f(control_points[i].x, control_points[i].y, control_points[i].z);
				glVertex3f(control_points[i + 1].x, control_points[i + 1].y, control_points[i + 1].z);
			}
			else if (i == control_points.size() - 1) {
				glVertex3f(control_points[i].x, control_points[i].y, control_points[i].z);
				glVertex3f(control_points[i - 1].x, control_points[i - 1].y, control_points[i - 1].z);
			}
			else {
				glVertex3f(control_points[i - 1].x, control_points[i - 1].y, control_points[i - 1].z);
				glVertex3f(control_points[i].x, control_points[i].y, control_points[i].z);
				glVertex3f(control_points[i + 1].x, control_points[i + 1].y, control_points[i + 1].z);
			}
			glEnd();
		}
	}

	// Draw Normals
	if (renderNormals) {
		glColor3f(1.f, 0.5F, 1.f);
		glBegin(GL_LINES);
		for (int f = 0; f <= curve.detail; ++f) {
			float3 point = curve.ValueAt(f / (float)curve.detail);
			float3 normal = curve.NormalAt(f / (float)curve.detail);
			glVertex3f(point[0], point[1], point[2]);
			glVertex3f(point[0] + normal[0], point[1] + normal[1], point[2] + normal[2]);
		}
		glEnd();
	}

	glEnable(GL_LIGHTING);
}

Curve::Curve(const float3& begin, const float3& end, const float3& position)
{
	this->position = position;

	control_points.push_back(begin + position);
	control_points.push_back(begin + float3(5.f, 10.f, 0.f) + position);
	control_points.push_back(end + float3(-5.f, 10.f, 0.f) + position);
	control_points.push_back(end + position);

	control_points_normals.push_back(float3::unitY());
	control_points_normals.push_back(float3::unitY());
}

float3 Curve::ValueAt(float at)
{
	at = Clamp01<float>(at);

	if (at == 0) {
		return control_points.front();
	}
	else if (at == 1) {
		return control_points.back();
	}

	int num_segments = (control_points.size() - 1) / 3;
	float ratio_segments = 1 / (float)num_segments;
	int current_segment = at / ratio_segments + 1;
	int indexControl = (current_segment - 1) * 3;
	
	float time = Maths::Map(at, ratio_segments * (current_segment - 1), ratio_segments * current_segment, 0.0F, 1.0F);

	return CubicCurve(control_points[indexControl], control_points[indexControl + 1], 
		control_points[indexControl + 2], control_points[indexControl + 3], time);
}

float3 Curve::NormalAt(float at)
{
	at = Clamp01<float>(at);

	if(at <= 0.f)
		return control_points_normals.front().Normalized();
	else if (at >= 1.f)
		return control_points_normals.back().Normalized();

	int num_segments = (control_points.size() - 1) / 3;
	int indexControl = at / (1.f / (float)num_segments);
	
	return Quat::SlerpVector(
		control_points_normals[indexControl].Normalized(), 
		control_points_normals[indexControl + 1].Normalized(), 
		at * (float)num_segments - indexControl // t in Slerp must be from 0 to 1 in that segment so we have to multiply the global t with the number of segments and substract the index of the point
	).Normalized();
}

const std::vector<float3>& Curve::GetControlPoints()
{
	return control_points;
}

const std::vector<float3>& Curve::GetControlPointsNormals()
{
	return control_points_normals;
}

void Curve::SetControlPointAt(int index, const float3& value)
{
	control_points[index] = value;
}

void Curve::SetControlPointNormalAt(int index, const float3& value)
{
	control_points_normals[index] = value;
}

void Curve::SetDetail(int detail)
{
	this->detail = detail;
}

void Curve::AddSegment(bool begin)
{
	float3 newPoint = float3::zero();
	float3 tensor1 = float3::zero();
	float3 tensor2 = float3::zero();

	if (begin) {
		newPoint = control_points.front() + float3(-10.f, 0.f, 0.f);
		tensor1 = newPoint + float3(2.5F, 10.f, 0.f);
		tensor2 = control_points.front() + float3(-2.5F, 0.f, 0.f);
		
		control_points.insert(control_points.begin(), tensor2);
		control_points.insert(control_points.begin(), tensor1);
		control_points.insert(control_points.begin(), newPoint);

		control_points_normals.insert(control_points_normals.begin(), float3::unitY());
	}
	else {
		newPoint = control_points.back() + float3(10.f, 0.f, 0.f);
		tensor1 = control_points.back() + float3(2.5F, 10.f, 0.f);
		tensor2 = newPoint + float3(-2.5F, 10.f, 0.f);

		control_points.push_back(tensor1);
		control_points.push_back(tensor2);
		control_points.push_back(newPoint);

		control_points_normals.push_back(float3::unitY());
	}
}

void Curve::InsertControlPoint(int index)
{
	float3 begin = control_points[index];
	float3 end = control_points[index + 3];

	float3 mid = (begin + end) * 0.5F;
	float3 tensor1 = mid + float3(-5.f, 10.f, 0.f);
	float3 tensor2 = mid + float3(5.f, 10.f, 0.f);

	std::vector<float3> points;
	points.push_back(tensor1);
	points.push_back(mid);
	points.push_back(tensor2);

	control_points_normals.insert(control_points_normals.begin() + (index/3) + 1, float3::unitY());

	control_points.insert(control_points.begin() + index + 2, points.begin(), points.end());
}

void Curve::RemoveControlPoint(int index)
{
	if (index == 0) {
		control_points.erase(control_points.begin());
		control_points.erase(control_points.begin());
		control_points.erase(control_points.begin());

		control_points_normals.erase(control_points_normals.begin());
	}
	else if (index == control_points.size() - 1) {
		control_points.pop_back();
		control_points.pop_back();
		control_points.pop_back();

		control_points_normals.pop_back();
	}
	else {
		control_points_normals.erase(control_points_normals.begin() + (index / 3));

		control_points.erase(control_points.begin() + --index);
		control_points.erase(control_points.begin() + index);
		control_points.erase(control_points.begin() + index);
	}
}

void Curve::UpdatePosition(const float3& new_position)
{
	float3 difference = new_position - position;
	position = new_position;

	for (auto item = control_points.begin(); item != control_points.end(); ++item) {
		(*item) += difference;
	}
}

void Curve::SetPoints(const std::vector<float3>& controlPoints, const std::vector<float3>& normalPoints)
{
	control_points.clear();
	control_points_normals.clear();

	control_points.assign(controlPoints.begin(), controlPoints.end());
	control_points_normals.assign(normalPoints.begin(), normalPoints.end());
}

float3 Curve::QuadraticCurve(const float3& a, const float3& b, const float3& c, float t)
{
	float3 p0 = Lerp(a, b, t);
	float3 p1 = Lerp(b, c, t);

	return Lerp(p0, p1, t);
}

float3 Curve::CubicCurve(const float3& a, const float3& b, const float3& c, const float3& d, float t)
{
	float3 p0 = QuadraticCurve(a, b, c, t);
	float3 p1 = QuadraticCurve(b, c, d, t);

	return Lerp(p0, p1, t);
}
