#include "Application.h"
#include "ComponentParticleSystem.h"
#include "ParticleSystem.h"
#include "ComponentTransform.h"
#include "Time.h"
#include "imgui/imgui.h"
#include "MathGeoLib/include/Math/float3.h"
#include "ModuleResources.h"
#include "ModuleUI.h"
#include "ReturnZ.h"
#include "imgui/imgui_internal.h"
#include "PanelProject.h"
#include "ResourceMaterial.h"
#include "ResourceMesh.h"
#include "ResourceModel.h"
#include "Optick/include/optick.h"
#include "ComponentMaterial.h"
#include "mmgr/mmgr.h"

ComponentParticleSystem::ComponentParticleSystem(GameObject* parent) : Component(parent)
{
	type = ComponentType::PARTICLES;
	particleSystem = new ParticleSystem();

	ComponentTransform* transform = (ComponentTransform*)game_object_attached->GetComponent(ComponentType::TRANSFORM);
	
}

ComponentParticleSystem::~ComponentParticleSystem()
{
	if (particleSystem != nullptr)
	{
		delete particleSystem; 
		particleSystem = nullptr; 
	}

	/*if(texture != nullptr)
	{
		texture->DecreaseReferences();
		texture = nullptr;
	}

	if (selected_texture != nullptr)
		selected_texture = nullptr;*/
	
	/*if (material != nullptr)
		material = nullptr;*/
}

void ComponentParticleSystem::OnPlay()
{
	particleSystem->Restart();
}

void ComponentParticleSystem::OnPause()
{
	particleSystem->Pause();
}

void ComponentParticleSystem::OnStop()
{
	particleSystem->Stop();
}

void ComponentParticleSystem::OnEmitterPlay()
{
	particleSystem->emmitter.Play();
}

void ComponentParticleSystem::OnEmitterStop()
{
	particleSystem->emmitter.Stop();
}

void ComponentParticleSystem::PreUpdate()
{
	OPTICK_EVENT();
	ComponentTransform* transform = (ComponentTransform*)game_object_attached->GetComponent(ComponentType::TRANSFORM);

	particleSystem->emmitter.SetPosition(transform->GetGlobalPosition());
	particleSystem->emmitter.SetRotation(transform->GetGlobalRotation());

	if(particleSystem->isPlaying())
		particleSystem->PreUpdate(Time::GetCurrentDT());
}

void ComponentParticleSystem::Update()
{
	OPTICK_EVENT();
	if (particleSystem->isPlaying())
		particleSystem->Update(Time::GetCurrentDT());
}

void ComponentParticleSystem::PostUpdate()
{
	OPTICK_EVENT();
	if (particleSystem->isPlaying())
		particleSystem->PostUpdate(Time::GetCurrentDT());
}

void ComponentParticleSystem::DrawScene(ComponentCamera* camera)
{
	OPTICK_EVENT();

	if (game_object_attached->selected)
	{
		Draw();
		DebugDraw();
	}
}

void ComponentParticleSystem::DrawGame(ComponentCamera* camera)
{
	OPTICK_EVENT();

	Draw();
}

void ComponentParticleSystem::DebugDraw()
{
	OPTICK_EVENT();

	if (drawEmmitter) 
		particleSystem->DrawEmmitter();

}

void ComponentParticleSystem::Draw()
{
	OPTICK_EVENT();

	particleSystem->DrawParticles();

}

void ComponentParticleSystem::OnEnable()
{
	particleSystem->Restart();
}

void ComponentParticleSystem::OnDisable()
{
	particleSystem->Pause(); 
}

bool ComponentParticleSystem::DrawInspector()
{
	
	//texture = material->material->texture;
	
	
	static bool check;

	ImGui::PushID(this);
	check = enabled;
	if (ImGui::Checkbox("##CmpActive", &check)) {
		ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
		enabled = check;
	}
	ImGui::PopID();
	ImGui::SameLine();

	

	if (ImGui::CollapsingHeader("Particle System", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{

		static bool cntl_Z = true; // CTRLZ Checking

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// ----------------------------------- Emmitter ----------------------------------------
		


		if (ImGui::TreeNodeEx("Emmitter", ImGuiTreeNodeFlags_Bullet  | ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ParticleEmmitter* emmitter = &particleSystem->emmitter;

			ImGui::Spacing();
			ImGui::Checkbox("Draw Emmitter", &drawEmmitter);

			if (ImGui::TreeNode("Shape Options"))
			{
				shapeSelected = (int)particleSystem->emmitter.GetShape();
				if (ImGui::Combo("Shape", &shapeSelected, "Sphere\0Hemisphere\0Cube\0Cone\0\0")) { emmitter->SetShape((Emmitter_Shape)shapeSelected); }

				switch (emmitter->GetShape())
				{
				case Emmitter_Shape::Sphere:
				{
					float radius = emmitter->GetRadius();
					if (ImGui::DragFloat("Radius", &radius, 0.2f, 0.0f, FLT_MAX)) { emmitter->SetRadius(radius); }

					int zoneSelected = (int)emmitter->GetZone();
					if (ImGui::Combo("Emmission Zone", &zoneSelected, "Center\0Volume\0\0")) { emmitter->SetZone((Emmitter_Zone)zoneSelected); }
				}
				break;

				case Emmitter_Shape::Cone:
				{
					// Not inner radius yet

					float outRadius = emmitter->GetOutRadius();
					if (ImGui::DragFloat("Outter Radius", &outRadius, 0.2f, 0.0f, FLT_MAX)) { emmitter->SetOutRadius(outRadius); }

					float distance = emmitter->GetDistance();
					if (ImGui::DragFloat("Distance", &distance, 0.2f, 0.0f, FLT_MAX)) { emmitter->SetDistance(distance); }
				}
				break;

				case Emmitter_Shape::Cube:
					ImGui::DragFloat3("Cube Size", emmitter->GetCubeSize(), 1.0f, 0.0f, FLOAT_INF);
					break;

				}

				ImGui::TreePop();
			}


			float duration = emmitter->GetMaxLife();
			if (ImGui::DragFloat("Duration", &duration, 1.0F, 0.0F, FLT_MAX)) 
			{ 
				if (cntl_Z)
				 ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this); 

				cntl_Z = false;
				emmitter->SetMaxLife(duration);
			}
			else if (!cntl_Z && ImGui::IsMouseReleased(0)) {
				cntl_Z = true;
			}

			bool loop = emmitter->GetLoop();
			if (ImGui::Checkbox("Looping", &loop)) { emmitter->SetLoop(loop); }

			float delay = emmitter->GetDelay();
			if (ImGui::DragFloat("Delay", &delay, 0.2f, 0.00f, FLT_MAX)) { emmitter->SetDelay(delay); }

			float spawnRate = emmitter->GetSpawnRate();
			if (ImGui::DragFloat("Spawn Rate", &spawnRate, 0.2f, 0.00f, FLT_MAX)) { emmitter->SetSpawnRate(spawnRate); }


			float3 pos = particleSystem->emmitter.GetRelativePosition();
			if (ImGui::DragFloat3("Position", (float*)&pos)) { emmitter->SetRelativePosition(pos); }

			float3 rot = particleSystem->emmitter.GetRelativeRotation();
			if (ImGui::DragFloat3("Rotation", (float*)&rot)) { emmitter->SetRelativeRotation(rot); }

			if (ImGui::TreeNodeEx("Bursts"))
			{
				for (int i = 0; i < emmitter->bursts.size(); ++i)
				{
					ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
					if (ImGui::TreeNodeEx((void*)(intptr_t)i, base_flags, "Burst %i", i))
					{
						ImGui::DragFloat("Time", &emmitter->bursts[i].timeToBurst, 0.1f, 0, emmitter->GetMaxLife());
						ImGui::DragInt("Particles", &emmitter->bursts[i].partsToInstantiate, 1.0f, 0, MAX_PARTICLES_TO_BURST);
						if (ImGui::Button("Remove")) emmitter->RemoveBurst(i);
						ImGui::TreePop();

					}
				}

				if (ImGui::Button("Add Burst")) { emmitter->bursts.push_back(Burst()); }

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// ----------------------------------- Particle ----------------------------------------

		if (ImGui::TreeNodeEx("Particle", ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::DragFloat("Life Time", &particleSystem->particleInfo.maxLifeTime, 1.0f, 0.0f, FLT_MAX);
			ImGui::DragFloat("Speed", (float*)&particleSystem->particleInfo.speed, 0.2f);

			if (ImGui::RadioButton("Global", &transformSelected, 0)) { particleSystem->particleInfo.globalTransform = true; }
			ImGui::SameLine();
			if (ImGui::RadioButton("Local", &transformSelected, 1)) { particleSystem->particleInfo.globalTransform = false; }

			// Initial State || Final State
			if (ImGui::TreeNodeEx("Start State", ImGuiTreeNodeFlags_DefaultOpen))
			{

				if (ImGui::ColorPicker4("Color", (float*)&particleSystem->particleInfo.color, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_AlphaPreview))
				{
					if (particleSystem->material != nullptr)
						particleSystem->material->color = particleSystem->particleInfo.color;
				}
				else
				{
					if (particleSystem->material != nullptr)
						particleSystem->particleInfo.color = particleSystem->material->color;
				}

				/*if (particleSystem->material != nullptr) {

					if(ImGui::ColorPicker4("Color", (float*)&particleSystem->particleInfo.color, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_AlphaPreview))
						particleSystem->material->color = particleSystem->particleInfo.color;
				}
				else
					ImGui::ColorPicker4("Color", (float*)&particleSystem->particleInfo.color, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_AlphaPreview);*/

				//ImGui::DragFloat("Size", (float*)&particleSystem->particleInfo.size, 0.1f, 0.0f, FLT_MAX);

				if (ImGui::Checkbox("3D Size", &particleSystem->particleInfo.size3DStart)){
					particleSystem->particleInfo.size = particleSystem->particleInfo.size3D.x;
					particleSystem->particleInfo.size3D = float3(particleSystem->particleInfo.size, particleSystem->particleInfo.size, particleSystem->particleInfo.size);

					particleSystem->endInfo.size = particleSystem->endInfo.size3D.x;
					particleSystem->endInfo.size3D = float3(particleSystem->endInfo.size, particleSystem->endInfo.size, particleSystem->endInfo.size);
					
				}
				ImGui::Spacing();
				if (particleSystem->particleInfo.size3DStart)
				{
					ImGui::Text("3D Size: "); ImGui::SameLine(200, 15);
					if (ImGui::DragFloat3("##size3D", (float*)&particleSystem->particleInfo.size3D, 0.1f, 0.0f, FLT_MAX)) {
					}
				}
				else
				{
					if (ImGui::DragFloat("Size", (float*)&particleSystem->particleInfo.size, 0.1f, 0.0f, FLT_MAX)) {
						particleSystem->particleInfo.size3D = float3(particleSystem->particleInfo.size, particleSystem->particleInfo.size, particleSystem->particleInfo.size);
					}
				
				}


				if (ImGui::Checkbox("Start 3D Rotation", &particleSystem->particleInfo.axisRot3DStart))
				{
					particleSystem->particleInfo.angle3D = math::float3(0.0f, 0.0f, particleSystem->particleInfo.angle3D.z);
				}
				ImGui::Spacing();
				if (particleSystem->particleInfo.axisRot3DStart)
				{
					ImGui::Text("Start 3D Rotation: "); ImGui::SameLine(200, 15);
					ImGui::DragFloat3("##rot3D", (float*)&particleSystem->particleInfo.angle3D, 0.1f, 0.0f, 360.0f);
				}
				else
					ImGui::DragFloat("Start Rotation", (float*)&particleSystem->particleInfo.angle3D.z, 0.1f, 0.0f, 360.0f);


				//ImGui::DragFloat3("Start Rotation 3D", (float*)&particleSystem->particleInfo.angle3D, 0.1f, 0.0f, 360.0f);
				ImGui::DragFloat3("Gravity", (float*)&particleSystem->particleInfo.force);
				ImGui::Checkbox("Change Over Time", &particleSystem->particleInfo.changeOverLifeTime);

				ImGui::TreePop();
			}

			if (particleSystem->particleInfo.changeOverLifeTime)
			{
				if (ImGui::TreeNodeEx("Final State", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::DragFloat("Final Time", &particleSystem->particleInfo.changedTime, 0.1f, 0.0f, particleSystem->particleInfo.maxLifeTime);
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::ColorPicker4("Color", (float*)&particleSystem->endInfo.color,ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_AlphaPreview);

					/*if (particleSystem->material != nullptr)
						ImGui::ColorPicker4("Color", (float*)&particleSystem->endInfo.color,
							ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_AlphaPreview);
					else
						ImGui::ColorPicker4("Color", (float*)&particleSystem->endInfo.color,
							ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_AlphaPreview);*/

					ImGui::DragFloat("Speed", &particleSystem->endInfo.speed, 0.2f);
					//ImGui::DragFloat("Size", (float*)&particleSystem->endInfo.size, 0.1f, 0.0f, FLT_MAX);
					
					
					/*if (ImGui::Checkbox("3D Size", &particleSystem->particleInfo.size3DStart))
					{
						particleSystem->endInfo.size = particleSystem->endInfo.size3D.x;
						particleSystem->endInfo.size3D = float3(particleSystem->endInfo.size, particleSystem->endInfo.size, particleSystem->endInfo.size);
					}*/
					ImGui::Spacing();
					if (particleSystem->particleInfo.size3DStart)
					{
						ImGui::Text("3D Size: "); ImGui::SameLine(200, 15);
						ImGui::DragFloat3("##size3D", (float*)&particleSystem->endInfo.size3D, 0.1f, 0.0f, FLT_MAX);
					}
					else
					{
						if (ImGui::DragFloat("Size", (float*)&particleSystem->endInfo.size, 0.1f, 0.0f, FLT_MAX)) {
							particleSystem->endInfo.size3D = float3(particleSystem->endInfo.size, particleSystem->endInfo.size, particleSystem->endInfo.size);
						}

					}
					
					
					
					
					
					
					
					
					
					
					
					
					
					ImGui::DragFloat3("Gravity", (float*)&particleSystem->endInfo.force);
					ImVec2 size = ImGui::GetItemRectSize();
					if (ImGui::Button("Equalize Values", size))
						particleSystem->endInfo = particleSystem->particleInfo;


					ImGui::Spacing(); ImGui::Spacing();

					ImGui::Checkbox("##pptActiveRotation", &particleSystem->particleInfo.rotateOverTime);
					ImGui::SameLine();

					if (ImGui::TreeNodeEx("Rotation Over Time", ImGuiTreeNodeFlags_Framed))
					{

						ImGui::Spacing();

						if (!particleSystem->particleInfo.rotateOverTime)
						{
							ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
							ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
						}

						if (ImGui::Checkbox("Separate Axis", &particleSystem->particleInfo.axisRot3D))
						{
							particleSystem->particleInfo.angularVelocity3D = math::float3(0.0f, 0.0f, particleSystem->particleInfo.angularVelocity3D.z);
							particleSystem->particleInfo.angularAcceleration3D = math::float3(0.0f, 0.0f, particleSystem->particleInfo.angularAcceleration3D.z);
						}

						ImGui::Spacing();
						ImGui::Spacing();

						if (particleSystem->particleInfo.axisRot3D)
						{
							ImGui::Text("Angular Velocity 3D: "); ImGui::SameLine(240, 15);
							ImGui::DragFloat3("##angular vel3D", (float*)&particleSystem->particleInfo.angularVelocity3D, 0.1f, 0.0f, 360.0f);
							ImGui::Spacing();
							ImGui::Text("Angular Acceleration 3D: "); ImGui::SameLine(240, 15);
							ImGui::DragFloat3("##angular accl3D", (float*)&particleSystem->particleInfo.angularAcceleration3D, 0.1f, 0.0f, 360.0f);
						}

						else
						{
							ImGui::Text("Angular Velocity "); ImGui::SameLine(230, 15);
							ImGui::DragFloat("##angular vel", (float*)&particleSystem->particleInfo.angularVelocity3D.z, 0.1f, 0.0f, 360.0f);
							ImGui::Spacing();
							ImGui::Text("Angular Acceleration "); ImGui::SameLine(230, 15);
							ImGui::DragFloat("##angular acc", (float*)&particleSystem->particleInfo.angularAcceleration3D.z, 0.1f, 0.0f, 360.0f);
						}



						if (!particleSystem->particleInfo.rotateOverTime)
						{
							ImGui::PopItemFlag();
							ImGui::PopStyleVar();
						}



						ImGui::TreePop();
					}


					ImGui::TreePop();
				}
			}
			/*else
				particleSystem->material->shaderInputs.particleShaderProperties.color = particleSystem->material->shaderInputs.particleShaderProperties.start_color;*/

			ImGui::TreePop();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// ----------------------------------- Renderer ----------------------------------------

		


		if (ImGui::TreeNodeEx("Renderer", ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_DefaultOpen))
		{
			
		
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Spacing();
			
			ImGui::TextColored(ImVec4(1.0f, 0.54f, 0.0f, 1.0f), "PARTICLE BILLBOARD:");
			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Text("Orientation Mode ");
			ImGui::SameLine(200, 15);
			if (ImGui::Combo("Billboard", &bbTypeSelected, "Screen Aligned\0World Aligned\0Axially Aligned\0Velocity Aligned\0Mesh\0None\0\0"))
			{
				particleSystem->SetBillboardType((BillboardType)bbTypeSelected);
			}

			if (particleSystem->bbType == BillboardType::VELOCITY)
			{
				particleSystem->mesh_mode = false;

				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::Text("Length Scale: "); ImGui::SameLine(200, 15);
				ImGui::DragFloat("##Length Scale", &particleSystem->particleInfo.lengthScale, 0.0f, 100.0f);
				
				ImGui::Spacing();

				ImGui::Text("Speed Scale: "); ImGui::SameLine(200, 15);
				ImGui::DragFloat("##Speed Scale", &particleSystem->particleInfo.speedScale, 0.0f, 100.0f);
			}
			else if (particleSystem->bbType == BillboardType::MESH)
			{
				particleSystem->mesh_mode = true;

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::TextColored(ImVec4(1.0f, 0.54f, 0.0f, 1.0f), "PARTICLE MESH:");
				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::Text("Mesh ");
				ImGui::SameLine(200, 15);

				if (ImGui::Combo("Mesh", &meshTypeSelected, "Cube\0Sphere\0Rock\0Dodecahedron\0Octahedron\0Torus\0Icosahedron\0Custom\0None\0\0"))
				{
					particleSystem->SetMeshType((PARTICLE_MESH)meshTypeSelected);
					particleSystem->CreateParticleMesh(particleSystem->meshType);
				}
				

				if (particleSystem->meshType == PARTICLE_MESH::CUSTOM)
				{
					ImGui::Spacing();
					ImGui::Text("Custom Mesh ");
					ImGui::SameLine(200, 15);

					if (!particleSystem->meshes.empty())
						ImGui::Button(particleSystem->meshes.front()->name.data(), { ImGui::GetWindowWidth() * 0.25F , 0 });
					else
						ImGui::Button("none", { ImGui::GetWindowWidth() * 0.25F , 0 });

					//if (node != nullptr && node->type == FileDropType::MODEL3D) {
					//	std::string path = App->file_system->GetPathWithoutExtension(node->path + node->name);
					//	path += "_meta.alien";

					//	u64 ID = App->resources->GetIDFromAlienPath(path.data());

					//	std::string meta_path = LIBRARY_MODELS_FOLDER + std::to_string(ID) + ".alienModel";

					//	if (!App->resources->CreateNewModelInstanceOf(meta_path.data())) { // if it goes here it is because this file wasn't imported yet, so import it now

					//		App->importer->LoadModelFile(std::string(node->path + node->name).data(), nullptr);
					//		ID = App->resources->GetIDFromAlienPath(path.data());
					//		meta_path = LIBRARY_MODELS_FOLDER + std::to_string(ID) + ".alienModel";
					//		App->resources->CreateNewModelInstanceOf(meta_path.data());
					//	}
					//}

					if (ImGui::BeginDragDropTarget()) {
						const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_PROJECT_NODE, ImGuiDragDropFlags_SourceNoDisableHover);

						if (payload != nullptr && payload->IsDataType(DROP_ID_PROJECT_NODE)) {
							FileNode* node = *(FileNode**)payload->Data;
							if (node != nullptr && node->type == FileDropType::MODEL3D) {

								if (particleSystem->meshType == PARTICLE_MESH::CUSTOM)
								{
									std::string path = App->file_system->GetPathWithoutExtension(node->path + node->name);
									path += "_meta.alien";

									u64 ID = App->resources->GetIDFromAlienPath(path.data());

									std::string meta_path = LIBRARY_MODELS_FOLDER + std::to_string(ID) + ".alienModel";


									if (ID != 0)
									{
										ResourceModel* model = (ResourceModel*)App->resources->GetResourceWithID(ID);
										if (model != nullptr) {
											
											vector<ResourceMesh*> tmp = model->meshes_attached;
											//particleSystem->SetMesh(meshes.back());
											particleSystem->SetMeshes(tmp);
										}
									}

									//if (!App->resources->CreateNewModelInstanceOf(meta_path.data())) { // if it goes here it is because this file wasn't imported yet, so import it now

									//	App->importer->LoadModelFile(std::string(node->path + node->name).data(), nullptr);
									//	ID = App->resources->GetIDFromAlienPath(path.data());
									//	meta_path = LIBRARY_MODELS_FOLDER + std::to_string(ID) + ".alienModel";
									//	App->resources->CreateNewModelInstanceOf(meta_path.data());
									//}
								}
							}
						}
						ImGui::EndDragDropTarget();
					}
						ImGui::SameLine();
						if (ImGui::Button("Delete", { ImGui::GetWindowWidth() * 0.15F , 0 }))
						{
							particleSystem->RemoveMesh();
						}
				}
			}
			else
			{
				particleSystem->mesh_mode = false;
				particleSystem->particleInfo.lengthScale = 1.0f;
				particleSystem->particleInfo.speedScale = 0.0f;
			}
			
			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::TextColored(ImVec4(1.0f, 0.54f, 0.0f, 1.0f), "PARTICLE MATERIAL: ");
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Text("Material");
			ImGui::SameLine(200, 15);

			if (particleSystem->material != nullptr)
				ImGui::Button(particleSystem->material->name.data(), { ImGui::GetWindowWidth() * 0.25F , 0 });
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
								particleSystem->SetMaterial(mat);
							}
						}
					}
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::SameLine();
			
			if (ImGui::Button("Delete", { ImGui::GetWindowWidth() * 0.15F , 0 }))
			{
			
				if (particleSystem->material != nullptr) {
					particleSystem->RemoveMaterial();
				}
			}
			



			ImGui::Spacing();
			ImGui::Spacing();

			//ImGui::Text("Particle Texture ");

			//static ResourceTexture* selected_texture = nullptr;

			//if (texture != nullptr)
			//{
			//	ImGui::Spacing();
			//	ImGui::Spacing();

			//	static bool check;
			//	check = texture_activated;
			//	if (ImGui::Checkbox("Texture Active", &check)) {
			//		//ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			//		texture_activated = check;
			//	}

			//	ImGui::Text("Texture Size:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", texture->width);
			//	ImGui::SameLine(); ImGui::Text("x"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", texture->height);
			//	ImGui::Text("Path:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%s", texture->GetAssetsPath());
			//	ImGui::Text("References:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", texture->references);
			//	ImGui::Image((ImTextureID)texture->id, {140 ,140 });

			//	ImGui::Spacing();

			//	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
			//	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
			//	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
			//	if (ImGui::Button("Delete", { 60,20 })) {
			//		//ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			//		texture->DecreaseReferences();
			//		texture = nullptr;
			//		particleSystem->texture = texture;
			//		selected_texture = nullptr;	

			//		
			//	}

			//	ImGui::PopStyleColor();
			//	ImGui::PopStyleColor();
			//	ImGui::PopStyleColor();

			//	ImGui::SameLine(80, 15);
			//	if (ImGui::Button("Change Texture", { 120,20 })) {
			//		change_texture_menu = true;
			//		selected_texture = texture;
			//	}
			//	ImGui::Spacing();
			//}

			//else {
			//	

			//	ImGui::SameLine(200, 15);
			//	if (ImGui::Button("Add Texture", { 120,20 })) {
			//		change_texture_menu = true;
			//		selected_texture = texture;
			//	}
			//	ImGui::SameLine();
			//	ImGui::TextColored(ImVec4(1.0f, 0.54f, 0.0f, 1.0f), "No Texture assigned");
			//	ImGui::Spacing();
			//}


			/*if (change_texture_menu) 
			{
				TextureBrowser();
			}*/

			
			
			ImGui::Spacing();

			//enable_anim = false;

			// Add Spritesheet texture
			ImGui::Checkbox("##pptActiveAnim", &enable_anim);
			ImGui::SameLine();

			if (ImGui::TreeNodeEx("Texture Animation", ImGuiTreeNodeFlags_Framed))
			{

				ImGui::Spacing();

				if (!enable_anim)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Text("Animation Speed: "); ImGui::SameLine(200, 15);
				//ImGui::SliderFloat("##Animation Speed", &animSpeed, 0.0f, 5.0);
				ImGui::DragFloat("##Animation Speed", &animSpeed, 0.0f, 5.0f);
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Text("Rows: "); ImGui::SameLine(200, 15);
				if (ImGui::SliderInt("##Rows", &texRows, 0, 10)) { endFrame = (texRows * texColumns) - 1; }
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Text("Columns: "); ImGui::SameLine(200, 15);
				if (ImGui::SliderInt("##Columns", &texColumns, 0, 10)) { endFrame = (texRows * texColumns) - 1; }
				
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Text("Start Frame: "); ImGui::SameLine(200, 15);
				ImGui::InputInt("##startf", &startFrame, 1,1);
				ImGui::Text("End Frame: "); ImGui::SameLine(200, 15);
				ImGui::InputInt("##endf", &endFrame, 1, 1);
				//ImGui::SameLine(535, 15);
				ImGui::Spacing();
				ImGui::Spacing();

				if (ImGui::Button("Calculate UV", { 120,20 }))
				{
					particleSystem->CalculateParticleUV(texRows, texColumns, animSpeed, startFrame, endFrame);
				}
				ImGui::SameLine();


				if (ImGui::Button("Reset", { 120,20 })  ||  !enable_anim)
				{
					particleSystem->ResetParticleUV();
					texRows = 1;
					texColumns = 1;
					startFrame = 0;
					endFrame = (texRows * texColumns) - 1;
				}
				ImGui::SameLine();
				ImGui::Text("Texture UV Frames: "); ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", particleSystem->particleInfo.frames.size());

				ImGui::Spacing();
				ImGui::Spacing();

				if (!enable_anim)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::TreePop();
			}

			
			static bool enable_blend = false;

			ImGui::Checkbox("##pptActiveBlend", &enable_blend);
			ImGui::SameLine();
			
			if (ImGui::TreeNodeEx("Blending Options", ImGuiTreeNodeFlags_Framed))
			{
				
				ImGui::Spacing();

				if (!enable_blend)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				ImGui::TextColored(ImVec4(1.0f, 0.54f, 0.0f, 1.0f), "Transparency:");
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Text("Alpha test: "); ImGui::SameLine(200, 15);
				ImGui::SliderFloat("##alpha test", (float*)&particleSystem->alpha_test, 0.1f, 1.0f);

				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::TextColored(ImVec4(1.0f, 0.54f, 0.0f, 1.0f), "Blending Color:");
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Text("Source ");
				ImGui::SameLine(200, 15);
				if (ImGui::Combo("##Source", &funcTypeSource, "ZERO\0ONE\0SRC_COLOR\0ONE_MINUS_SRC_COLOR\0DST_COLOR\0ONE_MINUS_DST_COLOR\0SRC_ALPHA\0ONE_MINUS_SRC_ALPHA\0DST_ALPHA\0ONE_MINUS_DST_ALPHA\0CONSTANT_COLOR\0ONE_MINUS_CONSTANT_COLOR\0CONSTANT_ALPHA\0ONE_MINUS_CONSTANT_ALPHA\0\0"))
					particleSystem->funcBlendSource = (FunctionBlendType)funcTypeSource;
				ImGui::Text("Destination ");
				ImGui::SameLine(200, 15);
				if (ImGui::Combo("##Destination", &funcTypeDest, "ZERO\0ONE\0SRC_COLOR\0ONE_MINUS_SRC_COLOR\0DST_COLOR\0ONE_MINUS_DST_COLOR\0SRC_ALPHA\0ONE_MINUS_SRC_ALPHA\0DST_ALPHA\0ONE_MINUS_DST_ALPHA\0CONSTANT_COLOR\0ONE_MINUS_CONSTANT_COLOR\0CONSTANT_ALPHA\0ONE_MINUS_CONSTANT_ALPHA\0\0"))
					particleSystem->funcBlendDest = (FunctionBlendType)funcTypeDest;

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Text("Blend Equation: ");
				ImGui::SameLine(200, 15);
				if (ImGui::Combo("##Equation Mode", &eqTypeSelected, "ADD\0SUBTRACT\0REVERSE SUBTRACT\0\0"))
					particleSystem->eqBlend = (EquationBlendType)eqTypeSelected;

				if (!enable_blend)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::TreePop();
			}
			

			ImGui::TreePop();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (ImGui::TreeNodeEx("Serialization", ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Text("Save Particle System ");
			ImGui::SameLine(200, 15);
			if (ImGui::Button("Save", { 120,20 })) {
				SaveParticles();
			}
				
			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Text("Load Particle System ");
			ImGui::SameLine(200, 15);
			if (ImGui::Button("Load", { 120,20 })) {
				LoadParticles();
			}

				

		
			if (ImGui::Button("Restart Particle System")) particleSystem->ResetSystem();


		ImGui::TreePop();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (particleSystem->material != nullptr) {

			if (particleSystem->material == particleSystem->default_material)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}


			particleSystem->material->DisplayMaterialOnInspector();


			if (particleSystem->material == particleSystem->default_material)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
		}
		
	}

	return true;
}

//void ComponentParticleSystem::TextureBrowser()
//{
//	ImGui::OpenPopup("Textures Loaded");
//	ImGui::SetNextWindowSize({ 522,585 });
//
//	if (ImGui::BeginPopupModal("Textures Loaded", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
//		ImGui::Spacing();
//		ImGui::NewLine();
//		ImGui::SameLine(190);
//		ImGui::Text("Texture Selected");
//		ImGui::Text("");
//		ImGui::SameLine(170);
//
//		if (selected_texture != nullptr) {
//			ImGui::Image((ImTextureID)selected_texture->id, { 150,150 });
//			ImGui::Spacing();
//			ImGui::Text("");
//			ImGui::SameLine(150);
//
//			ImGui::Text("Texture Size:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", selected_texture->width);
//			ImGui::SameLine(); ImGui::Text("x"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", selected_texture->height);
//			ImGui::Text("");
//			ImGui::SameLine(150);
//			ImGui::Text("References:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", selected_texture->references);
//			ImGui::Text("");
//			ImGui::SameLine(112);
//			ImGui::Text("Path:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%s", selected_texture->GetAssetsPath());
//		}
//		ImGui::Spacing();
//
//		if (ImGui::BeginChild("##TexturesSelectorChild", { 492,285 }, true, ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
//			ImGui::Columns(3, 0, false);
//			ImGui::SetColumnWidth(0, 156);
//			ImGui::SetColumnWidth(1, 156);
//			ImGui::SetColumnWidth(2, 156);
//
//			std::vector<Resource*>::iterator item = App->resources->resources.begin();
//			for (; item != App->resources->resources.end(); ++item) {
//				if (*item != nullptr && (*item)->GetType() == ResourceType::RESOURCE_TEXTURE && static_cast<ResourceTexture*>(*item)->is_custom) {
//
//					if ((*item)->NeedToLoad())
//						(*item)->IncreaseReferences();
//
//					ImGui::ImageButton((ImTextureID)static_cast<ResourceTexture*>(*item)->id, { 140,140 });
//					if (ImGui::IsItemClicked()) {
//						selected_texture = static_cast<ResourceTexture*>(*item);
//					}
//					ImGui::NextColumn();
//				}
//			}
//
//			ImGui::EndChild();
//		}
//		ImGui::Spacing();
//		ImGui::Text("");
//		ImGui::SameLine(377);
//		if (ImGui::Button("Apply", { 120,20 })) {
//			ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
//			texture = selected_texture;
//			particleSystem->texture = texture;
//			selected_texture = nullptr;
//			change_texture_menu = false;
//
//			std::vector<Resource*>::iterator item = App->resources->resources.begin();
//			for (; item != App->resources->resources.end(); ++item) {
//				if (*item != nullptr && (*item)->GetType() == ResourceType::RESOURCE_TEXTURE && static_cast<ResourceTexture*>(*item)->is_custom) {
//
//					if (*item != texture)
//						(*item)->DecreaseReferences();
//				}
//			}
//		}
//		ImGui::SameLine(237);
//		if (ImGui::Button("Cancel", { 120,20 })) {
//			selected_texture = nullptr;
//			change_texture_menu = false;
//
//			std::vector<Resource*>::iterator item = App->resources->resources.begin();
//			for (; item != App->resources->resources.end(); ++item) {
//				if (*item != nullptr && (*item)->GetType() == ResourceType::RESOURCE_TEXTURE && static_cast<ResourceTexture*>(*item)->is_custom) {
//						(*item)->DecreaseReferences();
//				}
//			}
//		}
//		ImGui::EndPopup();
//	}
//}

//void ComponentParticleSystem::SetTexture(ResourceTexture* tex)
//{
//	texture = tex;
//	particleSystem->texture = texture;
//
//}
//
//void ComponentParticleSystem::SetMaterial(ResourceMaterial* mat)
//{
//	if (mat == nullptr)
//		return;
//	
//	if (material != nullptr)
//	{
//		material->DecreaseReferences();
//		//material = nullptr;
//	}
//
//	material = mat;
//	material->IncreaseReferences();
//}

void ComponentParticleSystem::Play()
{
	particleSystem->Play();
}

void ComponentParticleSystem::Pause()
{
	particleSystem->Pause();
}

void ComponentParticleSystem::Restart()
{
	particleSystem->Restart();
}

ParticleSystem* ComponentParticleSystem::GetSystem()
{
	return particleSystem;
}

void ComponentParticleSystem::SaveComponent(JSONArraypack* to_save)
{
	// --------------- General Info -------------------- //
	to_save->SetNumber("Type", (int)type);
	to_save->SetString("ID", std::to_string(ID).data());

	// ----------------------- Billboard Info ----------------------- //

	to_save->SetNumber("Billboard", (int)particleSystem->bbType);

	// --------------- Particle System Start Info -------------------- //

	// Position
	to_save->SetFloat3("Start.Position", particleSystem->particleInfo.position);
	// Rotation
	to_save->SetQuat("Start.Rotation", particleSystem->particleInfo.rotation);
	// Global / Local
	to_save->SetBoolean("Start.Global", particleSystem->particleInfo.globalTransform);
	// Velocity
	to_save->SetFloat3("Start.Velocity", particleSystem->particleInfo.velocity);
	// Force
	to_save->SetFloat3("Start.Force", particleSystem->particleInfo.force);
	// Speed
	to_save->SetNumber("Start.Speed", (float)particleSystem->particleInfo.speed);
	// Color
	to_save->SetFloat4("Start.Color", particleSystem->particleInfo.color);

	// 3D SizeStart
	to_save->SetBoolean("Start.SizeStart3D", particleSystem->particleInfo.size3DStart);
	// Size
	to_save->SetNumber("Start.Size", (float)particleSystem->particleInfo.size);
	// Size3D
	to_save->SetFloat3("Start.Size3D", particleSystem->particleInfo.size3D);
	// LightColor
	to_save->SetFloat4("Start.LightColor", particleSystem->particleInfo.lightColor);
	// MaxLifeTime
	to_save->SetNumber("Start.MaxLifeTime", (float)particleSystem->particleInfo.maxLifeTime);
	// AxisRot
	to_save->SetBoolean("Start.AxisRotationStart", particleSystem->particleInfo.axisRot3DStart);
	// Angle Rotation
	to_save->SetFloat3("Start.Angle3D", particleSystem->particleInfo.angle3D);
	// changeOverLifeTime
	to_save->SetBoolean("Start.ChangeOverLifeTime", particleSystem->particleInfo.changeOverLifeTime);
	// rotateOverLifeTime
	to_save->SetBoolean("Start.RotateOverLifeTime", particleSystem->particleInfo.rotateOverTime);

	// ----------------- Particle System End Info -------------------- //
	// Final Time
	to_save->SetNumber("End.FinalTime", particleSystem->particleInfo.changedTime);
	// Speed
	to_save->SetNumber("End.Speed", particleSystem->endInfo.speed);
	// Color
	to_save->SetFloat4("End.Color", particleSystem->endInfo.color);
	// Size
	to_save->SetNumber("End.Size", (float)particleSystem->endInfo.size);
	// Size3D
	to_save->SetFloat3("End.Size3D", particleSystem->endInfo.size3D);
	// LightColor
	to_save->SetFloat4("End.LightColor", particleSystem->endInfo.lightColor);
	// Force
	to_save->SetFloat3("End.Force", particleSystem->endInfo.force);
	// AxisRot
	to_save->SetBoolean("End.AxisRotation", particleSystem->particleInfo.axisRot3D);
	// Angular Velocity
	to_save->SetFloat3("End.AngularVelocity", particleSystem->particleInfo.angularVelocity3D);
	// Angular Acceleration
	to_save->SetFloat3("End.AngularAcceleration", particleSystem->particleInfo.angularAcceleration3D);


	// ---------------------- Emitter Info --------------------------- //

	// Shape
	to_save->SetNumber("Emmitter.Shape", (int)particleSystem->emmitter.GetShape());
	//Zone
	to_save->SetNumber("Emmitter.Zone", (int)particleSystem->emmitter.GetZone());
	//Distance
	to_save->SetNumber("Emmitter.Distance", particleSystem->emmitter.GetDistance());
	// Radius
	to_save->SetNumber("Emmitter.Radius", particleSystem->emmitter.GetRadius());
	// OutterRadius
	to_save->SetNumber("Emmitter.OutRadius", particleSystem->emmitter.GetOutRadius());
	// CubeSize
	to_save->SetFloat3("Emmitter.CubeSize", (float3)particleSystem->emmitter.GetCubeSize());
	// MaxLife
	to_save->SetNumber("Emmitter.MaxLife", particleSystem->emmitter.GetMaxLife());
	// CurrentLife
	to_save->SetNumber("Emmitter.CurrentLife", particleSystem->emmitter.GetCurrentLife());
	// SpawnRate
	to_save->SetNumber("Emmitter.SpawnRate", particleSystem->emmitter.GetSpawnRate());
	// Delay
	to_save->SetNumber("Emmitter.Delay", particleSystem->emmitter.GetDelay());
	// Loop
	to_save->SetBoolean("Emmitter.Loop", particleSystem->emmitter.GetLoop());
	// Position
	to_save->SetFloat3("Emmitter.Position", particleSystem->emmitter.GetPosition());
	// RelativePosition
	to_save->SetFloat3("Emmitter.RelativePosition", particleSystem->emmitter.GetRelativePosition());
	// Rotation
	to_save->SetFloat3("Emmitter.Rotation", particleSystem->emmitter.GetRotation());
	// Relative Rotation
	to_save->SetFloat3("Emmitter.RelativeRotation", particleSystem->emmitter.GetRelativeRotation());
	// Scale
	to_save->SetFloat3("Emmitter.Scale", particleSystem->emmitter.GetScale());

	// ------------------------ Burst Info --------------------------- //

	// Burst List size
	to_save->SetNumber("Bursts.Size", particleSystem->emmitter.bursts.size());

	std::string name;
	for (int i = 0; i < particleSystem->emmitter.bursts.size(); ++i)
	{
		// TimeBurst
		name = "Burst.TimeBurst-";
		name += i + '0';
		to_save->SetNumber(name.c_str(), particleSystem->emmitter.bursts[i].timeToBurst);

		// PartsInstanciate
		name = "Burst.PartsInstanciate-";
		name += i + '0';
		to_save->SetNumber(name.c_str(), particleSystem->emmitter.bursts[i].partsToInstantiate);
	}


	// ---------------------- Blending Info -------------------------- //

	// Source
	to_save->SetNumber("Blending.Source", (int)particleSystem->funcBlendSource);
	// Destination
	to_save->SetNumber("Blending.Destination", (int)particleSystem->funcBlendDest);
	// Equation
	to_save->SetNumber("Blending.Equation", (int)particleSystem->eqBlend);

	// --------------- Material Resource Info -------------------- //

	to_save->SetBoolean("HasMaterial", (particleSystem->material != nullptr) ? true : false);
	if (particleSystem->material != nullptr) {
		to_save->SetString("MaterialID", std::to_string(particleSystem->material->GetID()).data());
		//to_save->SetFloat4("Start.Color", particleSystem->material->shaderInputs.particleShaderProperties.color);
		//to_save->SetFloat3("Start.Color", particleSystem->material->shaderInputs.particleShaderProperties.start_color);
		//to_save->SetFloat3("End.Color", particleSystem->material->shaderInputs.particleShaderProperties.end_color);
	}

	// ------------------------ Animation Info ------------------------ //
	to_save->SetBoolean("HasAnimation", (enable_anim) ? true : false);
	// AnimSpeed
	to_save->SetNumber("Animation.AnimSpeed", (float)animSpeed);
	// Rows
	to_save->SetNumber("Animation.Rows", texRows);
	// Columns
	to_save->SetNumber("Animation.Columns", texColumns);
	// StartFrame
	to_save->SetNumber("Animation.StartFrame", startFrame);
	// EndFrame
	to_save->SetNumber("Animation.EndFrame", endFrame);


	to_save->SetBoolean("HasMesh", (particleSystem->mesh_mode) ? true : false);

	if (particleSystem->mesh_mode)
	{
		to_save->SetNumber("Mesh.MeshType", (int)particleSystem->meshType);
		to_save->SetNumber("Mesh.Size", (int)particleSystem->meshes.size());

		if (particleSystem->meshes.size() > 0)
		{
			for (int i = 0; i < particleSystem->meshes.size(); ++i)
			{
				std::string tmp = std::to_string(i);
				to_save->SetString(("Mesh.MeshesAttached.MeshID_" + tmp).data(), std::to_string(particleSystem->meshes.at(i)->GetID()).data());
			}
		}

	}

	// --------------- Deprecated -------------------- //
	/*to_save->SetBoolean("TextureEnabled", texture_activated);
	to_save->SetString("ID", std::to_string(ID));
	to_save->SetBoolean("HasTexture", (texture != nullptr) ? true : false);

	if (texture != nullptr) {
		to_save->SetString("TextureID", std::to_string(texture->GetID()));
	}*/

}

void ComponentParticleSystem::LoadComponent(JSONArraypack* to_load)
{
	// --------------- General Info -------------------- //
	//to_load->GetNumber("Type");
	ID = std::stoull(to_load->GetString("ID"));

	// ----------------------- Billboard Info ----------------------- //

	particleSystem->bbType = (BillboardType)(int)to_load->GetNumber("Billboard");
	bbTypeSelected = (int)to_load->GetNumber("Billboard");
	// --------------- Particle System Start Info -------------------- //

	// Position
	particleSystem->particleInfo.position = to_load->GetFloat3("Start.Position");
	// Rotation
	particleSystem->particleInfo.rotation = to_load->GetQuat("Start.Rotation");
	// Global / Local
	particleSystem->particleInfo.globalTransform = to_load->GetBoolean("Start.Global");
	transformSelected = particleSystem->particleInfo.globalTransform ? 0 : 1;
	// Velocity
	particleSystem->particleInfo.velocity = to_load->GetFloat3("Start.Velocity");
	// Force
	particleSystem->particleInfo.force = to_load->GetFloat3("Start.Force");
	// Speed
	particleSystem->particleInfo.speed = to_load->GetNumber("Start.Speed");
	// Color
	particleSystem->particleInfo.color = to_load->GetFloat4("Start.Color");
	try
	{
		// SizeStart
		particleSystem->particleInfo.size3DStart = to_load->GetBoolean("Start.SizeStart3D");
	}
	catch (...)
	{
		particleSystem->particleInfo.size3DStart = false;
	}
	// Size
	particleSystem->particleInfo.size = to_load->GetNumber("Start.Size");

	try {
		// Size 3D
		particleSystem->particleInfo.size3D = to_load->GetFloat3("Start.Size3D");
	}
	catch (...)
	{
		particleSystem->particleInfo.size3D = float3(1.f, 0.f, 0.f);
	}
	// LightColor
	particleSystem->particleInfo.lightColor = to_load->GetFloat4("Start.LightColor");
	// MaxLifeTime
	particleSystem->particleInfo.maxLifeTime = to_load->GetNumber("Start.MaxLifeTime");
	// AxisRot
	particleSystem->particleInfo.axisRot3DStart = to_load->GetBoolean("Start.AxisRotationStart");
	// Angle Rotation
	particleSystem->particleInfo.angle3D = to_load->GetFloat3("Start.Angle3D");
	// changeOverLifeTime
	particleSystem->particleInfo.changeOverLifeTime = to_load->GetBoolean("Start.ChangeOverLifeTime");
	// rotateOverLifeTime
	particleSystem->particleInfo.rotateOverTime = to_load->GetBoolean("Start.rotateOverLifeTime");


	// ----------------- Particle System End Info -------------------- //
	try {
	// Final Time
		particleSystem->particleInfo.changedTime = to_load->GetNumber("End.FinalTime");
	}
	catch (...)
	{
		particleSystem->particleInfo.changedTime = 5;
	}
	try {
	// Speed
		particleSystem->endInfo.speed = to_load->GetNumber("End.Speed");
	}
	catch (...)
	{
		particleSystem->endInfo.speed = 0.0f;
	}
	// Color
	particleSystem->endInfo.color = to_load->GetFloat4("End.Color");
	// Size
	particleSystem->endInfo.size = to_load->GetNumber("End.Size");
	try {
		// Size 3D
		particleSystem->particleInfo.size3D = to_load->GetFloat3("Start.Size3D");
	}
	catch (...)
	{
		particleSystem->particleInfo.size3D = float3(1.f, 0.f, 0.f);
	}
	// LightColor
	particleSystem->endInfo.lightColor = to_load->GetFloat4("End.LightColor");
	// Force
	particleSystem->endInfo.force = to_load->GetFloat3("End.Force");
	// AxisRot
	particleSystem->particleInfo.axisRot3D = to_load->GetBoolean("End.AxisRotation");
	// Angular Velocity
	particleSystem->particleInfo.angularVelocity3D = to_load->GetFloat3("End.AngularVelocity");
	// Angular Acceleration
	particleSystem->particleInfo.angularAcceleration3D = to_load->GetFloat3("End.AngularAcceleration");
	// ---------------------- Emitter Info --------------------------- //

	particleSystem->ResetSystem();

	// Shape
	particleSystem->emmitter.SetShape((Emmitter_Shape)(int)to_load->GetNumber("Emmitter.Shape"));
	//Zone
	particleSystem->emmitter.SetZone((Emmitter_Zone)(int)to_load->GetNumber("Emmitter.Zone"));
	//Distance
	particleSystem->emmitter.SetDistance(to_load->GetNumber("Emmitter.Distance"));
	// Radius
	particleSystem->emmitter.SetRadius(to_load->GetNumber("Emmitter.Radius"));
	// OutterRadius
	particleSystem->emmitter.SetOutRadius(to_load->GetNumber("Emmitter.OutRadius"));
	//CubeSize
	particleSystem->emmitter.SetCubeSize(to_load->GetFloat3("Emmitter.CubeSize"));
	// MaxLife
	particleSystem->emmitter.SetMaxLife(to_load->GetNumber("Emmitter.MaxLife"));
	//// CurrentLife
	//particleSystem->emmitter.SetCurrentLife(to_load->GetNumber("Emmitter.CurrentLife"));
	// SpawnRate
	particleSystem->emmitter.SetSpawnRate(to_load->GetNumber("Emmitter.SpawnRate"));
	// Delay
	particleSystem->emmitter.SetDelay(to_load->GetNumber("Emmitter.Delay"));
	// Loop
	particleSystem->emmitter.SetLoop(to_load->GetBoolean("Emmitter.Loop"));
	// Position
	particleSystem->emmitter.SetPosition(to_load->GetFloat3("Emmitter.Position"));
	// RelativePosition
	particleSystem->emmitter.SetRelativePosition(to_load->GetFloat3("Emmitter.RelativePosition"));
	// Rotation
	particleSystem->emmitter.SetRotation(to_load->GetFloat3("Emmitter.Rotation"));
	// Relative Rotation
	particleSystem->emmitter.SetRelativeRotation(to_load->GetFloat3("Emmitter.RelativeRotation"));
	// Scale
	particleSystem->emmitter.SetScale(to_load->GetFloat3("Emmitter.Scale"));

	// ------------------------ Burst Info --------------------------- //

	// Clean Burst List 
	particleSystem->emmitter.bursts.clear();

	// Burst List size
	int burstListSize = to_load->GetNumber("Bursts.Size");

	std::string name;
	for (int i = 0; i < burstListSize; ++i)
	{
		particleSystem->emmitter.bursts.push_back(Burst());

		// TimeBurst
		name = "Burst.TimeBurst-";
		name += i + '0';
		particleSystem->emmitter.bursts[i].timeToBurst = to_load->GetNumber(name.c_str());

		// PartsInstanciate
		name = "Burst.PartsInstanciate-";
		name += i + '0';
		particleSystem->emmitter.bursts[i].partsToInstantiate = to_load->GetNumber(name.c_str());
	}

	// ---------------------- Blending Info -------------------------- //

	// Source
	particleSystem->funcBlendSource = (FunctionBlendType)(int)to_load->GetNumber("Blending.Source");
	// Destination
	particleSystem->funcBlendDest = (FunctionBlendType)(int)to_load->GetNumber("Blending.Destination");
	// Equation
	particleSystem->eqBlend = (EquationBlendType)(int)to_load->GetNumber("Blending.Equation");


	// ---------------------- Resource Info -------------------------- //

	if (to_load->GetBoolean("HasMaterial")) {
		u64 ID = std::stoull(to_load->GetString("MaterialID"));
		particleSystem->SetMaterial((ResourceMaterial*)App->resources->GetResourceWithID(ID));
		//particleSystem->material->shaderInputs.particleShaderProperties.color = to_load->GetFloat4("Start.Color");

		//particleSystem->material->shaderInputs.particleShaderProperties.start_color = to_load->GetFloat3("Start.Color");
		//particleSystem->material->shaderInputs.particleShaderProperties.end_color = to_load->GetFloat3("End.Color");
	}
	ID = std::stoull(to_load->GetString("ID"));


	// ---------------------- Animation Info -------------------------- //

	if (to_load->GetBoolean("HasAnimation")) {
		enable_anim = to_load->GetBoolean("HasAnimation");
		// AnimSpeed
		animSpeed = to_load->GetNumber("Animation.AnimSpeed");
		// Rows
		texRows = (int)to_load->GetNumber("Animation.Rows");
		// Columns
		texColumns = (int)to_load->GetNumber("Animation.Columns");
		// StartFrame
		startFrame = (int)to_load->GetNumber("Animation.StartFrame");
		// EndFrame
		endFrame = (int)to_load->GetNumber("Animation.EndFrame");

		particleSystem->CalculateParticleUV(texRows, texColumns, animSpeed, startFrame, endFrame);
	}

	try {
		particleSystem->mesh_mode = to_load->GetBoolean("HasMesh");

		if (to_load->GetBoolean("HasMesh")) {

			particleSystem->meshType = (PARTICLE_MESH)(int)to_load->GetNumber("Mesh.MeshType");
			meshTypeSelected = (int)to_load->GetNumber("Mesh.MeshType");
			particleSystem->CreateParticleMesh((PARTICLE_MESH)particleSystem->meshType);

			if ((PARTICLE_MESH)particleSystem->meshType == PARTICLE_MESH::CUSTOM)
			{
				int size = (int)to_load->GetNumber("Mesh.Size");

				if (size > 0)
				{
					std::vector<ResourceMesh*> tmp_meshes;

					for (int i = 0; i < size; ++i)
					{
						std::string tmp = std::to_string(i);
						u64 ID = std::stoull(to_load->GetString(("Mesh.MeshesAttached.MeshID_" + tmp).data()));

						if (ID != 0)
						{
							ResourceMesh* mesh = (ResourceMesh*)App->resources->GetResourceWithID(ID);
							if (mesh != nullptr)
							{
								tmp_meshes.push_back(mesh);
							}
						}

					}
					particleSystem->SetMeshes(tmp_meshes);
				}
			}
		}

	}
	catch (...)
	{
		particleSystem->mesh_mode = false;
	}
	

	

	
	// ---------------------- Deprecated -------------------------- //
	/*texture_activated = to_load->GetBoolean("TextureEnabled");

	if (to_load->GetBoolean("HasTexture")) {
		u64 ID = std::stoull(to_load->GetString("TextureID"));
		texture = (ResourceTexture*)App->resources->GetResourceWithID(ID);
		if (texture != nullptr)
		{
			texture->IncreaseReferences();
			particleSystem->texture = texture;
		}
	}
	ID = std::stoull(to_load->GetString("ID"));*/


}



void ComponentParticleSystem::LoadParticles()
{
	OPENFILENAME to_load;

	static char filename[MAX_PATH];

	// get the current game directory
	static char curr_dir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, curr_dir);

	// add the local path of the scenes folder
	std::string dir = std::string(curr_dir + std::string("\\") + std::string("Assets\\Particles")).data();

	// fill eveything with 0  in order to avoid problems
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&to_load, sizeof(to_load));

	to_load.lStructSize = sizeof(to_load);
	to_load.hwndOwner = NULL;
	to_load.lpstrFilter = "alienParticles\0*.alienParticles";
	to_load.lpstrFile = filename;
	to_load.nMaxFile = MAX_PATH;
	to_load.lpstrTitle = "Load a .alienParticles";
	to_load.lpstrInitialDir = dir.data();
	to_load.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_EXPLORER;
	if (GetOpenFileNameA(&to_load))
	{
		SetCurrentDirectoryA(curr_dir);
		std::string name = filename;
		App->file_system->NormalizePath(name);


		JSON_Value* value = json_parse_file(name.data());
		JSON_Object* object = json_value_get_object(value);


		if (value != nullptr && object != nullptr)
		{
			JSONfilepack* particles = new JSONfilepack(name.data(), object, value);

			JSONArraypack* properties = particles->GetArray("ParticleSystem.Properties");

			if (properties != nullptr)
				LoadComponent(properties);

			delete particles;
		}
		else {
			LOG_ENGINE("Error loading particle system %s", name.data());
		}

		// last of all, refresh nodes because I have no idea if the user has created folders or moved things in the explorer. Users are bad people creating folders without using the alien engine explorer :(
		App->ui->panel_project->RefreshAllNodes();
	}
	else {
		SetCurrentDirectoryA(curr_dir);
	}
}


void ComponentParticleSystem::SaveParticles()
{
	OPENFILENAME to_save;

	static char filename[MAX_PATH];

	// get the current game directory
	static char curr_dir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, curr_dir);

	// add the local path of the scenes folder
	std::string dir = std::string(curr_dir + std::string("\\") + std::string("Assets\\Particles")).data();

	// fill eveything with 0  in order to avoid problems
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&to_save, sizeof(to_save));

	to_save.lStructSize = sizeof(to_save);
	to_save.hwndOwner = NULL;
	to_save.lpstrFilter = "alienParticles\0*.alienParticles";
	to_save.lpstrFile = filename;
	to_save.nMaxFile = MAX_PATH;
	to_save.lpstrTitle = "Save As New .alienParticles";
	to_save.lpstrInitialDir = dir.data();
	to_save.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_OVERWRITEPROMPT;
	if (GetSaveFileNameA(&to_save))
	{
		SetCurrentDirectoryA(curr_dir);

		std::string extension;
		App->file_system->SplitFilePath(filename, nullptr, nullptr, &extension);

		//ResourceScene* scene = new ResourceScene();
		std::string path;
		if (!App->StringCmp("alienParticles", extension.data())) {
			path = std::string(filename + std::string(".alienParticles")).data();
		}
		else {
			path = filename;
		}
		//scene->SetAssetsPath(path.data());
		//scene->CreateMetaData();
		//App->objects->SaveScene(scene);


		JSON_Value* value = json_value_init_object();
		JSON_Object* object = json_value_get_object(value);

		json_serialize_to_file_pretty(value, path.data());

		if (value != nullptr && object != nullptr)
		{
			JSONfilepack* particles = new JSONfilepack(path.data(), object, value);

			particles->StartSave();

			App->file_system->NormalizePath(path);
			std::string name = App->file_system->GetBaseFileName(path.data());

			particles->SetString("ParticleSystem.Name", name.data());
			JSONArraypack* properties = particles->InitNewArray("ParticleSystem.Properties");
			
			properties->SetAnotherNode();

			SaveComponent(properties);

			particles->FinishSave();
			delete particles;
		}
		else {
			LOG_ENGINE("Could not save particle system, fail when creating the file");
		}


		// last of all, refresh nodes because I have no idea if the user has created folders or moved things in the explorer. Users are bad people creating folders without using the alien engine explorer :(
		App->ui->panel_project->RefreshAllNodes();
	}
	else {
		SetCurrentDirectoryA(curr_dir);
	}
}



