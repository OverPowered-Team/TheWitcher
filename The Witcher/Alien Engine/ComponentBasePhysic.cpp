#include "ComponentBasePhysic.h"
#include "ComponentTransform.h"
#include "ComponentPhysics.h"
#include "GameObject.h"

ComponentBasePhysic::ComponentBasePhysic(GameObject* go) : Component(go)
{
	this->transform = go->GetComponent<ComponentTransform>();
	this->go = go;

	(go->HasComponent<ComponentPhysics>())
		? physics = go->GetComponent<ComponentPhysics>()
		: go->AddComponent(physics = new ComponentPhysics(go));

}

ComponentBasePhysic::~ComponentBasePhysic()
{
}
