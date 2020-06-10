#include "ComponentLightDirectional.h"
#include "glew/include/glew.h"
#include "GameObject.h"
#include "imgui/imgui.h"
#include "ComponentTransform.h"
#include "Application.h"
#include "ReturnZ.h"
#include "ModuleResources.h"
#include "ModuleRenderer3D.h"
#include "ModuleObjects.h"
#include "Viewport.h"
#include "ComponentMesh.h"
#include "ResourceMesh.h"
#include "Gizmos.h"
#include "mmgr/mmgr.h"

#include "Optick/include/optick.h"

#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"

ComponentLightDirectional::ComponentLightDirectional(GameObject* attach) : Component(attach)
{
	type = ComponentType::LIGHT_DIRECTIONAL;
	App->objects->directional_light_properites.push_back(&light_props);

	glGenFramebuffers(1, &light_props.depthMapFBO);
	glGenFramebuffers(1, &light_props.bakedepthMapFBO);

	light_props.light = this;
	light_props.enabled = enabled;
#ifndef GAME_VERSION
	bulb = new ComponentMesh(game_object_attached);
	bulb->mesh = App->resources->light_mesh;
	bulb->mesh->IncreaseReferences();
#endif

	InitFrameBuffers();
}

void ComponentLightDirectional::InitFrameBuffers()
{
	//dynamic shadows
	glBindFramebuffer(GL_FRAMEBUFFER, light_props.depthMapFBO);
	glGenTextures(1, &light_props.depthMap);
	glBindTexture(GL_TEXTURE_2D, light_props.depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, light_props.depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light_props.depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

#ifndef GAME_VERSION
	App->objects->debug_draw_list.emplace(this, std::bind(&ComponentLightDirectional::DrawScene, this));
#endif // !GAME_VERSION

	//static shadows
	glBindFramebuffer(GL_FRAMEBUFFER, light_props.bakedepthMapFBO);
	glGenTextures(num_of_static_shadowMap, light_props.bakedepthMap);
	for (uint i = 0; i < num_of_static_shadowMap; ++i) {
		glBindTexture(GL_TEXTURE_2D, light_props.bakedepthMap[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, light_props.bakedepthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light_props.bakedepthMap[0], 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ComponentLightDirectional::~ComponentLightDirectional()
{
#ifndef GAME_VERSION
	delete bulb;
#endif

	App->objects->directional_light_properites.remove(&light_props);

	glDeleteFramebuffers(1, &light_props.depthMapFBO);
	glDeleteFramebuffers(1, &light_props.bakedepthMapFBO);
	glDeleteTextures(1, &light_props.depthMap);
	glDeleteTextures(num_of_static_shadowMap, light_props.bakedepthMap);

#ifndef GAME_VERSION
	App->objects->debug_draw_list.erase(App->objects->debug_draw_list.find(this));
#endif // !GAME_VERSION
}

void ComponentLightDirectional::PostUpdate()
{
	OPTICK_EVENT();

	glm::mat4 projectionMatrix = glm::ortho(-sizefrustrum, sizefrustrum, -sizefrustrum, sizefrustrum,
		-sizefrustrum,
		sizefrustrum);

	light_props.projMat.Set(&projectionMatrix[0][0]);

	LightLogic();
}

void ComponentLightDirectional::BindForWriting(uint cascadeIndex)
{
	assert(cascadeIndex < num_of_static_shadowMap);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, light_props.bakedepthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light_props.bakedepthMap[cascadeIndex], 0);
}

void ComponentLightDirectional::BindForReading()
{
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, light_props.bakedepthMap[0]);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, light_props.bakedepthMap[1]);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, light_props.bakedepthMap[2]);
}


void ComponentLightDirectional::LightLogic()
{
	OPTICK_EVENT();

	ComponentTransform* transform = (ComponentTransform*)game_object_attached->GetComponent(ComponentType::TRANSFORM);
	light_props.position = float3(transform->GetGlobalPosition().x, transform->GetGlobalPosition().y, transform->GetGlobalPosition().z);
	light_props.direction = game_object_attached->transform->GetGlobalRotation().WorldZ();
}

void ComponentLightDirectional::DrawScene()
{
	OPTICK_EVENT();

	if (IsEnabled())
	{
		DrawIconLight();
		Gizmos::DrawLine(light_props.position, (light_props.position + light_props.direction * 3), Color::Green(), 0.1f);
	}

	//DrawLightFrustrum();
}

void ComponentLightDirectional::CalculateBakedViewMatrix()
{
	//Calculate ortographic light frustum position
	float3 far_position = (light_props.position + float3(2 * sizefrustrumbaked, 0 ,0)) / sizefrustrumbaked;
	float3 near_position = (light_props.position - float3(2 * sizefrustrumbaked, 0, 0)) / sizefrustrumbaked;
	float3 center_pos = (light_props.position) / sizefrustrumbaked ;

	//Calculate ortographic light frustum direction
	float3 center_light_dir = float3((center_pos.x - light_props.direction.x * distance_far_plane), (center_pos.y - light_props.direction.y * distance_far_plane), (center_pos.z - light_props.direction.z * distance_far_plane));
	float3 far_light_dir = float3((far_position.x - light_props.direction.x * distance_far_plane), (far_position.y - light_props.direction.y * distance_far_plane), (far_position.z - light_props.direction.z * distance_far_plane));
	float3 near_light_dir = float3((near_position.x - light_props.direction.x * distance_far_plane), (near_position.y - light_props.direction.y * distance_far_plane), (near_position.z - light_props.direction.z * distance_far_plane));

	//calculate ortographic light view matrix
	glm::mat4 centerviewMat = glm::lookAt(glm::vec3((float)center_pos.x, (float)center_pos.y, (float)-center_pos.z),
		glm::vec3((float)center_light_dir.x, (float)center_light_dir.y, (float)(-center_light_dir.z)),
		glm::vec3(0.0, 1.0, 0.0));

	glm::mat4 farviewMat = glm::lookAt(glm::vec3((float)far_position.x, (float)far_position.y, (float)-far_position.z),
		glm::vec3((float)far_light_dir.x, (float)far_light_dir.y, (float)(-far_light_dir.z)),
		glm::vec3(0.0, 1.0, 0.0));

	glm::mat4 nearviewMat = glm::lookAt(glm::vec3((float)near_position.x, (float)near_position.y, (float)-near_position.z),
		glm::vec3((float)near_light_dir.x, (float)near_light_dir.y, (float)(-near_light_dir.z)),
		glm::vec3(0.0, 1.0, 0.0));

	glm::mat4 projectionMatrixBaked = glm::ortho(-sizefrustrumbaked, sizefrustrumbaked, -sizefrustrumbaked, sizefrustrumbaked,
		-sizefrustrumbaked,
		sizefrustrumbaked);


	light_props.fake_position_baked[0] = near_light_dir;
	light_props.fake_position_baked[1] = center_light_dir;
	light_props.fake_position_baked[2] = far_light_dir;

	projMatrix.Set(&projectionMatrixBaked[0][0]);
	
	viewMatrix[0].Set(&nearviewMat[0][0]);
	viewMatrix[1].Set(&centerviewMat[0][0]);
	viewMatrix[2].Set(&farviewMat[0][0]);

}

bool ComponentLightDirectional::DrawInspector()
{
	static bool en;
	ImGui::PushID(this);
	en = enabled;
	if (ImGui::Checkbox("##CmpActive", &en)) {
		ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
		enabled = en;
		light_props.enabled = enabled;
	}
	ImGui::PopID();
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Light Directional", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		RightClickMenu("Light Directional");

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::PushID("printiconlight");
		ImGui::Checkbox("Print Icon", &print_icon);
		ImGui::PopID();

		// Parameters ---------
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Settings:");
		ImGui::DragFloat("Intensity", &light_props.intensity, 0.01f, 0.0f, 2.0f);
		ImGui::ColorEdit3("Ambient", light_props.ambient.ptr());
		ImGui::ColorEdit3("Diffuse", light_props.diffuse.ptr());
		ImGui::ColorEdit3("Specular", light_props.specular.ptr());

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Checkbox("Casts Shadows", &castShadows);
		ImGui::DragFloat("Shadow Intensity", &light_props.shadowIntensity, 0.01, 0.0, 1.0);

		ImGui::DragFloat("baked Shadow Map Size", &sizefrustrumbaked);

		ImGui::Text("Baked Depth Map");

		for(int i = 0; i < num_of_static_shadowMap; ++i)
			ImGui::Image((ImTextureID)light_props.bakedepthMap[i], ImVec2(300, 300));

		if (ImGui::Button("Bake Shadows"))
			bakeShadows = true;
	}
	else
		RightClickMenu("Light Directional");

	return true;
}

void ComponentLightDirectional::OnEnable()
{
}

void ComponentLightDirectional::OnDisable()
{
}

void ComponentLightDirectional::Clone(Component* clone)
{
	clone->enabled = enabled;
	clone->not_destroy = not_destroy;
	ComponentLightDirectional* light = (ComponentLightDirectional*)clone;
	light->renderer_id = renderer_id;
	light->print_icon = print_icon;
}

void ComponentLightDirectional::Reset()
{
	print_icon = true;
}

void ComponentLightDirectional::SetComponent(Component* component)
{
	if (component->GetType() == type) {

		ComponentLightDirectional* light = (ComponentLightDirectional*)component;

		renderer_id = light->renderer_id;
		print_icon = light->print_icon;
	}
}

void ComponentLightDirectional::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetBoolean("Enabled", enabled);
	to_save->SetString("ID", std::to_string(ID).data());
	to_save->SetBoolean("PrintIcon", print_icon);

	to_save->SetNumber("Intensity", float(light_props.intensity));
	to_save->SetFloat3("Position", float3(light_props.position));
	to_save->SetFloat3("Direction", float3(light_props.direction));
	to_save->SetFloat3("Ambient", float3(light_props.ambient));
	to_save->SetFloat3("Diffuse", float3(light_props.diffuse));
	to_save->SetFloat3("Specular", float3(light_props.specular));
	to_save->SetBoolean("CastShadows", castShadows);
	to_save->SetNumber("SizeBakedShadow", sizefrustrumbaked);
	to_save->SetNumber("sadowIntensity", float(light_props.shadowIntensity));

}

void ComponentLightDirectional::LoadComponent(JSONArraypack* to_load)
{
	enabled = to_load->GetBoolean("Enabled");
	ID = std::stoull(to_load->GetString("ID"));
	print_icon = to_load->GetBoolean("PrintIcon");

	light_props.intensity = (float)to_load->GetNumber("Intensity");
	light_props.position = to_load->GetFloat3("Position");
	light_props.direction = to_load->GetFloat3("Direction");
	light_props.ambient = to_load->GetFloat3("Ambient");
	light_props.diffuse = to_load->GetFloat3("Diffuse");
	light_props.specular = to_load->GetFloat3("Specular");
	castShadows = to_load->GetBoolean("CastShadows");

	try {
		sizefrustrumbaked = to_load->GetNumber("SizeBakedShadow");
		light_props.shadowIntensity = (float)to_load->GetNumber("shadowIntensity");

	}
	catch (...) {
		sizefrustrumbaked = 78.0f;
		light_props.shadowIntensity = 0.5f;
	}
}

void ComponentLightDirectional::DrawIconLight()
{
	OPTICK_EVENT();

	if (bulb != nullptr && print_icon)
	{
		ComponentTransform* transform = (ComponentTransform*)game_object_attached->GetComponent(ComponentType::TRANSFORM);
		float3 pos = transform->GetGlobalPosition();
		float4x4 matrix = float4x4::FromTRS({ pos.x - 0.133f, pos.y, pos.z }, transform->GetGlobalRotation(), { 0.2f, 0.18f, 0.2f });
		glDisable(GL_LIGHTING);
		Gizmos::DrawPoly(bulb->mesh, matrix, Color(0.0f, 255.0f, 0.0f));
		glEnable(GL_LIGHTING);
	}
}
