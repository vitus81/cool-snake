
#ifndef __MAIN_H__
#define __MAIN_H__

#include "raylib.h"

#include "constants.h"
#include "food.h"
#include "snake.h"

#include <deque>
#include <random>
#include <string>

#define NUM_FOOD_INSTANCES (4)

class Game {
public:
  Snake m_snake = Snake();
  std::vector<Food> m_food_vec;
  bool m_running = false;
  bool m_first_game = true;
  int m_score = 0;
  int m_last_score = 0;

  int m_last_food_type = -1;
  int m_streak_len = 1;
  int m_multiplier = 1;

  Game() {
    std::vector<Vector2> dummy;
    for (int i = 0; i < NUM_FOOD_INSTANCES; i++)
      m_food_vec.push_back(Food(dummy));
  };
  void draw();
  void update();
  void check_food_collision();
  void check_bonus_timeout();
  void check_game_over();
  void game_over();
  std::vector<Vector2> get_forbidden();

private:
};

#endif