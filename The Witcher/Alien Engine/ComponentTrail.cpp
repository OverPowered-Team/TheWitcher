#include "Application.h"
#include "ComponentTrail.h"
#include "ModuleObjects.h"
#include "Optick/include/optick.h"
#include "Trail.h"
#include "imgui/imgui_internal.h"
#include "PanelProject.h"
#include "Time.h"
#include "ModuleResources.h"

ComponentTrail::ComponentTrail(GameObject* parent) : Component(parent)
{
	type = ComponentType::TRAIL;
	trail = new Trail(this, parent);


	
#ifndef GAME_VERSION
	//App->objects->debug_draw_list.emplace(this, std::bind(&ComponentTrail::DrawScene, this));
#endif // !GAME_VERSION
}

ComponentTrail::~ComponentTrail()
{
	delete trail;
	trail = nullptr;
}

void ComponentTrail::PreUpdate()
{
	if (trail->isPlaying())
		trail->PreUpdate(Time::GetCurrentDT());
}

void ComponentTrail::Update()
{
	if(trail->isPlaying())
		trail->Update(Time::GetCurrentDT());
}

void ComponentTrail::PostUpdate()
{
	if (trail->isPlaying())
		trail->PostUpdate(Time::GetCurrentDT());
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
		//if (game_object_attached->selected)
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
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		if (ImGui::TreeNodeEx("Trail", ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_DefaultOpen))
		{

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Text("Direction Vector ");
		
			ImGui::SameLine();
			ImGui::SameLine(200, 15);
			if (ImGui::RadioButton("X", trail->vector == TrailVector::X))
			{
				trail->low = 4; trail->high = 5;
				trail->vector = TrailVector::X;
			} ImGui::SameLine();
			if (ImGui::RadioButton("Y", trail->vector == TrailVector::Y))
			{
				trail->low = 2; trail->high = 3;
				trail->vector = TrailVector::Y;
			} ImGui::SameLine();
			if (ImGui::RadioButton("Z", trail->vector == TrailVector::Z))
			{
				trail->low = 0; trail->high = 1;
				trail->vector = TrailVector::Z;
			}

			ImGui::Spacing();
		
			ImGui::Text("Time ");
			ImGui::SameLine(200, 15);
			ImGui::DragFloat("##Time", &trail->time, 10.0f, 0, 10000);
		
		
			ImGui::Spacing();

			ImGui::Text("LifeTime ");
			ImGui::SameLine(200, 15);
			ImGui::DragFloat("##Life Time", &trail->lifeTime, 10.0f, 0, 10000);


			ImGui::Spacing();

			ImGui::Text("Min Vertex Distance ");
			ImGui::SameLine(200, 15);
			ImGui::DragFloat("##Min Distance", &trail->minDistance, 0.01f, 0.0f, 10.0f);
		
			ImGui::Spacing();
		
			ImGui::Text("Emitting ");
			ImGui::SameLine(200, 15);
			ImGui::Checkbox("##Emitting", &trail->emitting);
			ImGui::Spacing();
		
			ImGui::Text("Orienting ");
			ImGui::SameLine(200, 15);
			ImGui::Checkbox("##Orient trail", &trail->orient);
			ImGui::Spacing();
		

			if (ImGui::Checkbox("Custom Width", &trail->customSpawn))
			{
				if (trail->customSpawn)
				{
					trail->originalSpawnBox = trail->_spawnBox;
				}
			}
			ImGui::Spacing();
		
			if (trail->customSpawn)
			{
				math::float3 size = trail->originalSpawnBox.Size();
				if (ImGui::DragFloat3("Spawn Size", &size.x, 1.0f, 0.0f, 0.0f, "%.0f"))
					trail->SetSpawnSize(size);

			}

			ImGui::Spacing();
			ImGui::Spacing();
			
			if (ImGui::ColorPicker4("Color", (float*)&trail->color, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_AlphaPreview))
			{
				if (trail->material != nullptr)
					trail->material->color = trail->color;
			}
			else
			{
				if (trail->material != nullptr)
					trail->color = trail->material->color;
			}
			ImGui::TreePop();

		}
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		if (ImGui::TreeNodeEx("Renderer", ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::TextColored(ImVec4(1.0f, 0.54f, 0.0f, 1.0f), "TRAIL BILLBOARD:");
			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Text("Orientation Mode ");
			ImGui::SameLine(200, 15);
			if (ImGui::Combo("Billboard", &bbTypeSelected, "View\0Transform Z\0None\0\0"))
			{
				
			}

			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::TextColored(ImVec4(1.0f, 0.54f, 0.0f, 1.0f), "TRAIL MATERIAL: ");
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Text("Material");
			ImGui::SameLine(200, 15);

			if (trail->material != nullptr)
				ImGui::Button(trail->material->name.data(), { ImGui::GetWindowWidth() * 0.25F , 0 });
			else
				ImGui::Button("none", { ImGui::GetWindowWidth() * 0.25F , 0 });


			if (ImGui::BeginDragDropTarget()) {
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_PROJECT_NODE, ImGuiDragDropFlags_SourceNoDisableHover);
				if (payload != nullptr && payload->IsDataType(DROP_ID_PROJECT_NODE)) {
					FileNode* node = *(FileNode**)payload->Data;
					if (node != nullptr && node->type == FileDropType::MATERIAL) {
						std::string path = App->file_system->GetPathWithoutExtension(node->path + node->name);
						path += "_meta.alien";
						u64 ID = App->resources->GetIDFromAlienPath(path.data());
						if (ID != 0) {
							ResourceMaterial* mat = (ResourceMaterial*)App->resources->GetResourceWithID(ID);
							if (mat != nullptr) {
								trail->SetMaterial(mat);
							}
						}
					}
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::SameLine();

			if (ImGui::Button("Delete", { ImGui::GetWindowWidth() * 0.15F , 0 }))
			{

				if (trail->material != nullptr) {
					trail->RemoveMaterial();
				}
			}

			ImGui::TreePop();
		}

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		

		if (trail->material != nullptr) {

			if (trail->material == trail->default_material)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}


			trail->material->DisplayMaterialOnInspector();


			if (trail->material == trail->default_material)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
		}
	}

	return true;
}

void ComponentTrail::SaveComponent(JSONArraypack* to_save)
{
	// --------------- General Info -------------------- //
	to_save->SetNumber("Type", (int)type);
	to_save->SetString("ID", std::to_string(ID).data());

	to_save->SetNumber("Trail.Vector", (int)trail->vector);
	to_save->SetNumber("Trail.Time", (float)trail->time);
	to_save->SetNumber("Trail.LifeTime", (float)trail->lifeTime);
	to_save->SetNumber("Trail.MinDistance", (float)trail->minDistance);

	to_save->SetBoolean("Trail.Emitting", trail->emitting);
	to_save->SetBoolean("Trail.Orienting", trail->orient);
	to_save->SetBoolean("Trail.CustomSpawn", trail->customSpawn);
	
	to_save->SetNumber("Trail.High", trail->high);
	to_save->SetNumber("Trail.Low", trail->low);

	to_save->SetFloat4("Trail.Color", trail->color);

	if(trail->customSpawn)
		to_save->SetFloat3("Trail.Width", trail->originalSpawnBox.Size());

	to_save->SetBoolean("HasMaterial", (trail->material != nullptr) ? true : false);
	if (trail->material != nullptr) {
		to_save->SetString("MaterialID", std::to_string(trail->material->GetID()).data());
	}

	
}

void ComponentTrail::LoadComponent(JSONArraypack* to_load)
{
	// --------------- General Info -------------------- //
	ID = std::stoull(to_load->GetString("ID"));


	trail->vector = (TrailVector)(int)to_load->GetNumber("Trail.Vector");
	trail->time = to_load->GetNumber("Trail.Time");
	trail->lifeTime = to_load->GetNumber("Trail.LifeTime");
	trail->minDistance = to_load->GetNumber("Trail.MinDistance");

	trail->emitting = to_load->GetBoolean("Trail.Emitting");
	trail->orient = to_load->GetBoolean("Trail.Orienting");
	trail->customSpawn = to_load->GetBoolean("Trail.CustomSpawn");

	trail->high = (int)to_load->GetNumber("Trail.High");
	trail->low = (int)to_load->GetNumber("Trail.Low");

	trail->color = to_load->GetFloat4("Trail.Color");

	if (trail->customSpawn)
	{
		trail->SetSpawnSize(to_load->GetFloat3("Trail.Width"));
	}
		
	if (to_load->GetBoolean("HasMaterial")) {
		u64 ID = std::stoull(to_load->GetString("MaterialID"));
		trail->SetMaterial((ResourceMaterial*)App->resources->GetResourceWithID(ID));
	}

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

Trail* ComponentTrail::GetTrail()
{
	return trail;
}
