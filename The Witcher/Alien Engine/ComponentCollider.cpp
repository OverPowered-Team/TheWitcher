#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleRenderer3D.h"
#include "ComponentCollider.h"
#include "ComponentTransform.h"
#include "ComponentRigidBody.h"
#include "ComponentScript.h"
#include "ComponentMesh.h"
#include "Time.h"
#include "GameObject.h"
#include "ReturnZ.h"
#include "Alien.h"
#include "mmgr/mmgr.h"

ComponentCollider::ComponentCollider(GameObject* go) : Component(go)
{
	// GameObject Components 
	transform = (transform == nullptr) ? game_object_attached->GetComponent<ComponentTransform>() : transform;
	if(transform)
		last_scale = transform->GetGlobalScale();

	std::vector<ComponentScript*> scripts = game_object_attached->GetComponents<ComponentScript>();

	for (ComponentScript* script : scripts)
	{
		if (script->need_alien == true)
		{
			alien_scripts.push_back(script);
		}
	}

	// Default values 
	center = float3::zero();
}

ComponentCollider::~ComponentCollider()
{
	App->SendAlienEvent(this, AlienEventType::COLLIDER_DELETED);

	if (internal_collider == false)
	{
		if (enabled == true)
		{
			if (rigid_body)
			{
				rigid_body->RemoveCollider();
			}

			App->physics->RemoveBody(aux_body);
			App->physics->RemoveDetector(detector);
		}

		delete detector;
		delete aux_body;
		delete shape;
	}
}

void ComponentCollider::Init()
{
	btTransform go_bullet_transform = ToBtTransform(transform->GetGlobalPosition() + GetWorldCenter(), transform->GetGlobalRotation());

	// Create elements
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0.F, nullptr, nullptr);
	aux_body = new btRigidBody(rbInfo);
	aux_body->setUserPointer(this);
	aux_body->setWorldTransform(go_bullet_transform);
	detector = new btPairCachingGhostObject();
	detector->setUserPointer(this);
	detector->setWorldTransform(go_bullet_transform);
	detector->setCollisionFlags(detector->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	// Create shape 
	CreateDefaultShape();
	aux_body->setCollisionShape(shape);
	detector->setCollisionShape(shape);

	// Add to world
	App->physics->AddBody(aux_body);
	App->physics->AddDetector(detector);

	// Search Rigid Body 
	ComponentRigidBody* rb_found = game_object_attached->GetComponent<ComponentRigidBody>();
	if (rb_found != nullptr && rb_found->IsEnabled())
		rb_found->AddCollider(this);

	// Default settings
	SetIsTrigger(false);
	SetBouncing(0.1f);
	SetFriction(0.5f);
	SetAngularFriction(0.1f);

	App->SendAlienEvent(this, AlienEventType::COLLIDER_ADDED);
}


// Colliders Functions --------------------------------

void ComponentCollider::SetCenter(float3 value)
{
	center = value;
	float3 current_scale = transform->GetGlobalScale();
	final_center = center.Mul(current_scale);
}

void ComponentCollider::SetIsTrigger(bool value)
{
	btRigidBody* current_body = (rigid_body) ? rigid_body->body : aux_body;
	is_trigger = value;
	(is_trigger)
		? current_body->setCollisionFlags(current_body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE)
		: current_body->setCollisionFlags(current_body->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
}

void ComponentCollider::SetBouncing(const float value)
{
	bouncing = math::Clamp(value, 0.f, 1.f);
	(rigid_body) 
		? rigid_body->body->setRestitution(bouncing)
		: aux_body->setRestitution(bouncing);
}

void ComponentCollider::SetFriction(const float value)
{
	friction = value;
	(rigid_body) 
		? rigid_body->body->setFriction(friction)
		: aux_body->setFriction(friction);
}

void ComponentCollider::SetAngularFriction(const float value)
{
	angular_friction = value;
	(rigid_body) 
		? rigid_body->body->setRollingFriction(angular_friction)
		: aux_body->setRollingFriction(angular_friction);
}

void ComponentCollider::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetBoolean("Enabled", enabled);
	to_save->SetNumber("Type", (int)type);

	to_save->SetFloat3("Center", center);
	to_save->SetBoolean("IsTrigger", is_trigger);
	to_save->SetNumber("Bouncing", bouncing);
	to_save->SetNumber("Friction", friction);
	to_save->SetNumber("AngularFriction", angular_friction);
}

void ComponentCollider::LoadComponent(JSONArraypack* to_load)
{
	enabled = to_load->GetBoolean("Enabled");
	if (enabled == false)
	{
		OnDisable();
	}

	SetCenter(to_load->GetFloat3("Center"));
	SetIsTrigger(to_load->GetBoolean("IsTrigger"));
	SetBouncing( to_load->GetNumber("Bouncing"));
	SetFriction(to_load->GetNumber("Friction"));
	SetAngularFriction( to_load->GetNumber("AngularFriction"));

	UpdateShape();
}

void ComponentCollider::SetScale(float3 scale)
{
	UpdateShape();
}

void ComponentCollider::Update()
{
	float3 pos = transform->GetGlobalMatrix().MulPos(GetWorldCenter());
	btTransform go_bullet_transform = ToBtTransform(GetWorldCenter(), transform->GetGlobalMatrix().RotatePart().RemoveScale2()); // New Method
	//btTransform go_bullet_transform = ToBtTransform(transform->GetGlobalPosition() + GetWorldCenter(), transform->GetGlobalRotation()); // Old Method

	if (internal_collider == false)
	{
		float3 current_scale = transform->GetGlobalScale();
		if (!last_scale.Equals(current_scale))
		{
			last_scale = current_scale;
			SetScale(last_scale);
		}
		
		if (rigid_body == nullptr)
		{
			aux_body->setWorldTransform(go_bullet_transform);
		}
	}

	
	detector->setWorldTransform(go_bullet_transform);
	detector->setActivationState(DISABLE_DEACTIVATION);

	if (!alien_scripts.empty() && Time::IsPlaying())
	{
		if (first_frame == false)
		{
			first_frame = true;
			return;
		}

		if (first_frame == true)
		{
			int num_obj_inside = 0;
			int test = 0;
			num_obj_inside = detector->getNumOverlappingObjects(); //num_obj_inside is set to 0xcdcdcdcd

			for (auto& x : collisions)
			{
				x.second = false;
			}

			for (int i = 0; i < num_obj_inside; ++i)
			{
				btCollisionObject* obj = detector->getOverlappingObject(i);
				btPairCachingGhostObject* ghost = dynamic_cast<btPairCachingGhostObject*>(obj);
				if (ghost)
				{
					ComponentCollider* coll = (ComponentCollider*)ghost->getUserPointer();
					std::map<ComponentCollider*, bool>::iterator search = collisions.find(coll);
					if (search != collisions.end() && coll != nullptr)
					{
						collisions[coll] = true;

						for (ComponentScript* script : alien_scripts)
						{
							Alien* alien = (Alien*)script->data_ptr;
							alien->OnTrigger(coll);
						}
					}
					else
					{
						collisions[coll] = true;

						for (ComponentScript* script : alien_scripts)
						{
							Alien* alien = (Alien*)script->data_ptr;
							alien->OnTriggerEnter(coll);
						}
					}

					++test;
				}
			}

			std::map<ComponentCollider*, bool>::iterator itr = collisions.begin();

			while (itr != collisions.end())
			{
				if (itr->second == false) {

					for (ComponentScript* script : alien_scripts)
					{
						Alien* alien = (Alien*)script->data_ptr;
						alien->OnTriggerExit(itr->first);
					}

					itr = collisions.erase(itr);
				}
				else {
					++itr;
				}
			}
		}
	}
}

void ComponentCollider::OnEnable()
{
	App->physics->AddBody(aux_body);
	App->physics->AddDetector(detector);

	// Search Rigid Body 
	ComponentRigidBody* rb = game_object_attached->GetComponent<ComponentRigidBody>();
	(rb != nullptr) ? rb->AddCollider(this) : rb = nullptr;
}

void ComponentCollider::OnDisable()
{
	if (rigid_body)
	{
		rigid_body->RemoveCollider();
	}

	App->physics->RemoveBody(aux_body);
	App->physics->RemoveDetector(detector); 
	
}

void ComponentCollider::DrawScene()
{
	if (enabled == true && game_object_attached->IsSelected() && App->physics->debug_physics == false)
	{
		App->physics->DrawCollider(this);
	}
}

void ComponentCollider::Reset()
{
	SetCenter(float3::zero());
	SetIsTrigger(false);
	SetBouncing(0.1f);
	SetFriction(0.5f);
	SetAngularFriction(0.1f);
}

bool ComponentCollider::DrawInspector()
{
	static bool check;
	check = enabled;

	ImGui::PushID(this);

	if (ImGui::Checkbox("##CmpActive", &check)) {
		enabled = check;
		if (!enabled)
			OnDisable();
		else
			OnEnable();
	}

	float3 current_center = center;
	bool current_is_trigger = is_trigger;
	float current_bouncing = bouncing;
	float current_friction = friction;
	float current_angular_friction = angular_friction;

	ImGui::SameLine();

	if (ImGui::CollapsingHeader(name.c_str(), &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Spacing();

		ImGui::Title("Layer");

		if (ImGui::BeginComboEx(std::string("##layers").c_str(), std::string(" " + App->physics->layers.at(layer)).c_str(), 200, ImGuiComboFlags_NoArrowButton))
		{
			for (int n = 0; n < App->physics->layers.size(); ++n)
			{
				bool is_selected = (layer == n);

				if (ImGui::Selectable(std::string("   " + App->physics->layers.at(n)).c_str(), is_selected))
				{
					layer = n;
				}

				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		ImGui::Title("Center", 1);			if (ImGui::DragFloat3("##center", current_center.ptr(), 0.05f)) { SetCenter(current_center); }

		DrawSpecificInspector();

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Title("Is Trigger", 1);		if (ImGui::Checkbox("##is_trigger", &current_is_trigger)) { SetIsTrigger(current_is_trigger); }
		ImGui::Spacing();
		ImGui::Title("Physic Material", 1); ImGui::Text("");
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Title("Bouncing", 2);	    if (ImGui::DragFloat("##bouncing", &current_bouncing, 0.01f, 0.00f, 1.f)) { SetBouncing(current_bouncing); }
		ImGui::Title("Linear Fric.", 2);	if (ImGui::DragFloat("##friction", &current_friction, 0.01f, 0.00f, FLT_MAX)) { SetFriction(current_friction); }
		ImGui::Title("Angular Fric.", 2);	if (ImGui::DragFloat("##angular_friction", &current_angular_friction, 0.01f, 0.00f, FLT_MAX)) { SetAngularFriction(current_angular_friction); }
		ImGui::Spacing();

	}

	ImGui::PopID();

	return true;
}

void ComponentCollider::HandleAlienEvent(const AlienEvent& e)
{
	switch (e.type)
	{
	case AlienEventType::SCRIPT_ADDED:
	{
		ComponentScript* script = (ComponentScript*)e.object;
		if (script->game_object_attached == game_object_attached && script->need_alien == true)
			alien_scripts.push_back(script);
		break;
	}
	case AlienEventType::SCRIPT_DELETED:
	{
		ComponentScript* script = (ComponentScript*)e.object;
		if (script->game_object_attached == game_object_attached)
			alien_scripts.remove(script);
		break;
	}
	case AlienEventType::COLLIDER_DELETED:
	{
		ComponentCollider* collider = (ComponentCollider*)e.object;

		if (!alien_scripts.empty() && Time::IsPlaying())
		{
			if (!collisions.empty() && collisions.find(collider) != collisions.end())
			{
				for (ComponentScript* script : alien_scripts)
				{
					Alien* alien = (Alien*)script->data_ptr;
					alien->OnTriggerExit(collider);
				}

				collisions.erase(collider);
			}
		}

		break;
	}
	default:
		break;
	}
}

float3 ComponentCollider::GetWorldCenter()
{
	return transform->GetGlobalMatrix().MulPos(center);
}

void ComponentCollider::AddToWorld()
{

}

void ComponentCollider::RemoveFromWorld()
{
}

