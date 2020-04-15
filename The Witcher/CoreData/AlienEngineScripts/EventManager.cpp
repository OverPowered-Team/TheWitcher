#include "GameManager.h"
#include "EventManager.h"
#include "PlayerManager.h"
#include "DialogueManager.h"

EventManager::EventManager() : Alien()
{
}

EventManager::~EventManager()
{
	eventPriorities.clear();
}

void EventManager::Start()
{
	eventPriorities =
	{
		{"Boss", 0},
		{"Narrative", 0},
		{"Enemies", 7},
		{"Default", 666}
	};
}

void EventManager::Update()
{
}

void EventManager::OnPlayerDead(PlayerController* player_dead)
{
	GameManager::manager->player_manager->OnPlayerDead(player_dead);
}

void EventManager::OnPlayerRevive(PlayerController* player_revived)
{
	GameManager::manager->player_manager->OnPlayerRevive(player_revived);
}

void EventManager::ReceiveDialogueEvent(Dialogue& dialogue, float delay) const
{
	/*bool c = (eventPriorities.find(dialogue.priority.c_str()) == eventPriorities.end());
	assert(!c && "Priority not valid");

	if (!c)
	{
		LOG("Priority not valid");
		return;
	}*/

	//eventPriorities.at(dialogue.priority)

	// TODO: send this to the dialogue script
	GameManager::manager->dialogue_manager->InputNewDialogue(dialogue);

}

void EventManager::ReceiveDialogueEvent(int index, float volume) const
{
	if (volume < 0.0f)
		volume = 0.0f;
	else if (volume > 1.0f)
		volume = 1.0f;

	GameManager::manager->dialogue_manager->InputNewDialogue(index, volume);
}
