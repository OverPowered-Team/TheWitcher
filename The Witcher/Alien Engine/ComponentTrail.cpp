#include "Application.h"
#include "ComponentTrail.h"
#include "ModuleObjects.h"
#include "Optick/include/optick.h"
#include "Trail.h"

ComponentTrail::ComponentTrail(GameObject* parent) : Component(parent)
{
	type = ComponentType::TRAIL;
	trail = new Trail(this, parent);
	
#ifndef GAME_VERSION
	App->objects->debug_draw_list.emplace(this, std::bind(&ComponentTrail::DrawScene, this));
#endif // !GAME_VERSION
}

ComponentTrail::~ComponentTrail()
{
}

void ComponentTrail::PreUpdate()
{
}

void ComponentTrail::Update()
{
}

void ComponentTrail::PostUpdate()
{

}

void ComponentTrail::DrawScene()
{
	DebugDraw();
}

void ComponentTrail::DrawGame()
{
	OPTICK_EVENT();

#ifndef GAME_VERSION
	if (App->objects->printing_scene)
	{
		if (game_object_attached->selected)
			Draw();
	}
	else
		Draw();
#else

	Draw();

#endif
}

void ComponentTrail::DebugDraw()
{
	

}

void ComponentTrail::Draw()
{
	trail->Draw();
}

void ComponentTrail::OnEnable()
{
}

void ComponentTrail::OnDisable()
{
}

bool ComponentTrail::DrawInspector()
{
	static bool check;

	ImGui::PushID(this);
	check = enabled;
	if (ImGui::Checkbox("##CmpActive", &check)) {
		//ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
		enabled = check;
	}
	ImGui::PopID();
	ImGui::SameLine();



	if (ImGui::CollapsingHeader("Trail System", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{

	}

	return true;
}

void ComponentTrail::SaveComponent(JSONArraypack* to_save)
{
}

void ComponentTrail::LoadComponent(JSONArraypack* to_load)
{
}

void ComponentTrail::Start()
{
	trail->Start();
}

void ComponentTrail::Stop()
{
	trail->Stop();
}

math::OBB ComponentTrail::GetOBBFromObject()
{
	return game_object_attached->GetGlobalOBB();
}
