
#ifndef __MAIN_H__
#define __MAIN_H__

#include "raylib.h"

#include "constants.h"
#include "food.h"
#include "snake.h"
#include "wall.h"

#include <deque>
#include <random>
#include <string>
#include <vector>

#define NUM_FOOD_INSTANCES (4)

#define SOUND_PATH (std::string("./resources/"))

class Game {
public:
  Snake m_snake = Snake();
  std::vector<Food> m_food_vec;
  std::vector<Wall> m_wall_vec;
  bool m_running = false;
  bool m_first_game = true;
  int m_score = 0;
  int m_last_score = 0;

  int m_last_food_type = -1;
  int m_streak_len = 0;

  int m_streak_multiplier = 1;
  int m_bonus_multiplier = 1;
  int m_total_multiplier = 1;
  int m_next_bonus_multiplier = 1;

  Sound m_food_sound;
  Sound m_gameover_sound;
  Sound m_bonus10x_sound;
  Sound m_wall_sound;
  Sound m_hourglass_sound;

  Game() {

    InitAudioDevice();
    m_food_sound = LoadSound((SOUND_PATH + "food.wav").c_str());
    m_gameover_sound = LoadSound((SOUND_PATH + "gameover.wav").c_str());
    m_bonus10x_sound = LoadSound((SOUND_PATH + "bonus10x.wav").c_str());
    m_wall_sound = LoadSound((SOUND_PATH + "wall.wav").c_str());
    m_hourglass_sound = LoadSound((SOUND_PATH + "hourglass.wav").c_str());;
    
    m_snake.reset();
    m_wall_vec.clear();

    std::vector<Vector2> dummy;
    for (int i = 0; i < NUM_FOOD_INSTANCES; i++)
      m_food_vec.push_back(Food(dummy));

    // // TEMP: create some walls for testing
    // m_wall_vec.push_back(Wall(Vector2{4,0}, 5, NORTH_SOUTH));
    // m_wall_vec.push_back(Wall(Vector2{0,12}, 6, EAST_WEST));
    // m_wall_vec.push_back(Wall(Vector2{13,18}, 12, NORTH_SOUTH));
    // m_wall_vec.push_back(Wall(Vector2{22,5}, 4, EAST_WEST));

    // // TEMP: random wall for testing
    // m_wall_vec.push_back(Wall(12, get_forbidden(), m_snake.get_head()));
    // m_wall_vec.push_back(Wall(9, get_forbidden(), m_snake.get_head()));
    // m_wall_vec.push_back(Wall(6, get_forbidden(), m_snake.get_head()));
    // m_wall_vec.push_back(Wall(3, get_forbidden(), m_snake.get_head()));
     
  };
  ~Game(){
    UnloadSound(m_food_sound);
    UnloadSound(m_gameover_sound);
    UnloadSound(m_bonus10x_sound);
    UnloadSound(m_wall_sound);
    UnloadSound(m_hourglass_sound);
    CloseAudioDevice();
  }
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