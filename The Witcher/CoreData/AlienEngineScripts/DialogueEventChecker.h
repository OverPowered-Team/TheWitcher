#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include <tuple>

struct Dialogue; 
class ALIEN_ENGINE_API DialogueEventChecker : public Alien {

public:
	DialogueEventChecker();
	virtual ~DialogueEventChecker();
	void Start();
	void Update();

	
private: 
	// TODO: add chekers

private:
	// vector of cheker functions and their associated dialogue data
	std::vector<std::tuple<void(*)(), Dialogue>> checkers; 

};

ALIEN_FACTORY DialogueEventChecker* CreateDialogueEventChecker() {
	DialogueEventChecker* alien = new DialogueEventChecker();
	// To show in inspector here

	return alien;
}


