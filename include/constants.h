#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include "raylib.h"

#include <deque>
#include <string>
#include <vector>

typedef std::vector<std::vector<int>> Game_grid_t;
typedef std::vector<float> Game_grid_linear_t;

struct game_globals_struct {
  Color green = {192, 230, 149, 255};
  Color dark_green = {74, 112, 106, 255};
  Color head_highlight_color = {54, 168, 151, 255};

  float head_highlight_timeout = 0.20;

  int cell_size = 30;
  int cell_count = 25;
  int offset = 75;
  std::string img_path = "./resources/";

  int prob_bonus = 5;
  int prob_surprise = 10;

  // whithin surprise
  int prob_surprise_bonus = 25;

  int score_regular = 1;
  int length_regular = 1;

  int score_bonus = 3;
  int length_bonus = 3; // decrease
  int timeout_bonus = 5; // seconds

  int surprise_bonus_multiplier = 10;

  bool sound_on = false;

  std::deque<Vector2> initial_player_pos = {Vector2{0, 9}, Vector2{-1, 9}, Vector2{-2, 9}};

};

#endif