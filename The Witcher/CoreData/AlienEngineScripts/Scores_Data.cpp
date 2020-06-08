#include "RelicBehaviour.h"
#include "Scores_Data.h"

int Scores_Data::player1_kills = 25;
int Scores_Data::player2_kills = 16;
std::vector<Relic*> Scores_Data::player1_relics;
std::vector<Relic*> Scores_Data::player2_relics;
bool Scores_Data::won_level1 = false;
bool Scores_Data::won_level2 = false;
bool Scores_Data::dead = false;
std::string Scores_Data::last_scene;
float3 Scores_Data::last_checkpoint_position = float3::inf();
int Scores_Data::player1_damage = 0;
int Scores_Data::player2_damage = 0;
int Scores_Data::coin_points_1 = 0;
int Scores_Data::coin_points_2 = 0;