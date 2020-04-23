#pragma once

struct Scores_Data 
{
	static int player1_kills;
	static int player2_kills;
	static bool won_level1;
	static bool won_level2;
	static bool dead;
};

int Scores_Data::player1_kills = 14;
int Scores_Data::player2_kills = 10;
bool Scores_Data::won_level1 = false;
bool Scores_Data::won_level2 = false;
bool Scores_Data::dead = false;