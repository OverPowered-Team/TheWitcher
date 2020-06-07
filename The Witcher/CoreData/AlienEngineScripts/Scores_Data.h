#pragma once
#include <string>
#include "../../MathGeoLib/include/Math/float3.h"
#include <vector>

class Relic;

struct Scores_Data 
{
	static int player1_kills;
	static int player2_kills;
	static bool won_level1;
	static bool won_level2;
	static std::string last_scene;
	static bool dead;
	static float3 last_checkpoint_position;
	static int player1_damage;
	static int player2_damage;
	static std::vector<Relic*> player1_relics;
	static std::vector<Relic*> player2_relics;
};

