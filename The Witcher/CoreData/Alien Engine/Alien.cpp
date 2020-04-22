#include "Alien.h"
#include "Application.h"
#include "ModuleObjects.h"
#include "GameObject.h"

Alien::Alien()
{
}

Alien::~Alien()
{
	App->objects->CancelInvokes(this);
}

bool Alien::IsScriptEnabled() const
{
	return (enabled == nullptr) ? false : *enabled;
}

void Alien::SetScriptEnable(const bool& enable)
{
	if (enable != *enabled) {
		*enabled = enable;
		if (enable) {
			OnEnable();
		}
		else {
			OnDisable();
		}
	}
}

const char* Alien::ToString()
{
	return game_object->ToString();
}

bool Alien::HasComponent(const ComponentType& component) const
{
	return game_object->HasComponent(component);
}

void Alien::Destroy(GameObject* obj)
{
	GameObject::Destroy(obj);
}

void Alien::DestroyInstantly(GameObject* obj)
{
	GameObject::DestroyInstantly(obj);
}

bool Alien::CompareTag(const char* tag_to_compare) const
{
	return strcmp(game_object->GetTag(), tag_to_compare) == 0;
}

void Alien::Invoke(std::function<void()> void_no_params_function, float seconds)
{
	if (void_no_params_function != nullptr) {
		App->objects->AddInvoke(void_no_params_function, seconds, this);
	}
}

void Alien::CancelInvoke()
{
	App->objects->CancelInvokes(this);
}

void Alien::InvokeRepeating(std::function<void()> void_no_params_function, float seconds_to_first_invoke, float seconds_between_each_call)
{
	if (void_no_params_function != nullptr) {
		App->objects->AddInvokeRepeating(void_no_params_function, seconds_to_first_invoke, seconds_between_each_call, this);
	}
}

//bool Alien::IsInvoking(std::function<void()> void_no_params_function)
//{
//	return App->objects->IsInvoking(void_no_params_function);
//}

