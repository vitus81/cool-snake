
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

extern game_globals_struct game_globals;

#define NUM_FOOD_INSTANCES (4)

#define SOUND_PATH (std::string("./resources/"))

class Game {
public:
  std::vector<Snake> m_snake_vec;
  std::vector<Food> m_food_vec;
  std::vector<Wall> m_wall_vec;
  Snake *m_snake; // pointer to player snake
  bool m_running = false;
  bool m_paused = false;
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
    m_bonus10x_sound = LoadSound((SOUND_PATH + "bonusx10.wav").c_str());
    m_wall_sound = LoadSound((SOUND_PATH + "wall.wav").c_str());
    m_hourglass_sound = LoadSound((SOUND_PATH + "hourglass.wav").c_str());

    m_wall_vec.clear();

    m_snake_vec.reserve(100); // prevent address change
    m_snake_vec.push_back(Snake(PLAYER, game_globals.initial_player_pos));
    m_snake_vec.push_back(spawn_snake());

    m_snake = &(m_snake_vec[0]);
    m_snake->reset();

    for (int i = 0; i < NUM_FOOD_INSTANCES; i++)
      m_food_vec.push_back(Food(get_forbidden()));

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
  ~Game() {
    UnloadSound(m_food_sound);
    UnloadSound(m_gameover_sound);
    UnloadSound(m_bonus10x_sound);
    UnloadSound(m_wall_sound);
    UnloadSound(m_hourglass_sound);
    CloseAudioDevice();
  }
  void draw();
  void write_text();
  void update();
  void check_food_collision();
  void check_bonus_timeout();
  void check_game_over();
  void game_over();
  std::vector<Vector2> get_forbidden();
  std::vector<Vector2> get_blocked();
  void spawn_in_seconds(float timeout);
  void check_spawn();
  Snake spawn_snake();
  Game_grid_t get_grid_blocked();

private:
  std::vector<float> m_spawn_queue;
};

#endif