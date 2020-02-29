#include "ListenerButton.h"

ListenerButton::ListenerButton() : Alien()
{
}

ListenerButton::~ListenerButton()
{
}

void ListenerButton::Start()
{
	ComponentButton* button = (ComponentButton*)GameObject::FindWithName("MyButton")->GetComponent(ComponentType::UI_BUTTON);
	if (button != nullptr) {
		button->AddListenerOnClick(std::bind(&ListenerButton::Exit, this));
	}
}

void ListenerButton::Update()
{
}

void ListenerButton::Exit()
{
	AlienEngine::QuitApp();
}
