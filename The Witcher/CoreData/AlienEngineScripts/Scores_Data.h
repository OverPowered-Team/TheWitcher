#pragma once
#include <string>
#include "../../MathGeoLib/include/Math/float3.h"
#include <vector>

class Relic;

struct Scores_Data 
{
	static bool won_level1;
	static bool won_level2;
	static std::string last_scene;
	static bool dead;
	static float3 last_checkpoint_position;
	static int player1_damage;
	static int player2_damage;
	static std::vector<Relic*> player1_relics;
	static std::vector<Relic*> player2_relics;
	static int total_player1_points;
	static int total_player2_points;
	static int coin_points_1; //Víctor's idea
	static int coin_points_2; //Víctor's idea
	static std::map<uint, uint> player1_kills;
	static std::map<uint, uint> player2_kills;
};

