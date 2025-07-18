/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit
https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "main.h"

#include "raylib.h"
#include "resource_dir.h" // utility header for SearchAndSetResourceDir

#include <iostream>

game_globals_struct game_globals;

std::vector<Texture> food_texture;
Texture hourglass_texture;
Texture question_texture;

void load_textures() {
  Image image;
  for (int i = 0; i < NUM_FOOD_TYPES; i++) {
    image = LoadImage(
        (game_globals.img_path + "food_" + std::to_string(i) + ".png").c_str());
    Texture txt = LoadTextureFromImage(image);
    food_texture.push_back(txt);
    UnloadImage(image);
  }
  image = LoadImage((game_globals.img_path + "food_bonus.png").c_str());
  hourglass_texture = LoadTextureFromImage(image);
  UnloadImage(image);
  image = LoadImage((game_globals.img_path + "food_surprise.png").c_str());
  question_texture = LoadTextureFromImage(image);
  UnloadImage(image);
}

void unload_textures() {
  for (auto &i : food_texture)
    UnloadTexture(i);
  food_texture.clear();
  UnloadTexture(hourglass_texture);
}

double last_update_time = 0;
bool event_triggered(double interval) {
  double current_time = GetTime();
  if (current_time - last_update_time >= interval) {
    last_update_time = current_time;
    return true;
  }
  return false;
}

int get_unique_food_id(int cat, int type) {
  return (5 * (cat == FOOD_SURPRISE) + 4 * (cat == FOOD_BONUS) + type);
};

void Game::draw() {
  for (auto &snake : m_snake_vec)
    snake.draw();
  for (auto &food : m_food_vec)
    food.draw();
  for (auto &wall : m_wall_vec)
    wall.draw();
}

std::vector<Vector2> Game::get_forbidden() {
  std::vector<Vector2> forbidden;
  for (auto &snake : m_snake_vec) {
    for (auto &i : snake.get_body()) {
      forbidden.push_back(i);
    }
  }
  for (auto &i : m_food_vec) {
    forbidden.push_back(i.get_position());
  }
  for (auto &wall : m_wall_vec) {
    for (auto &tile : wall.get_tiles()) {
      forbidden.push_back(tile);
    }
  }
  return forbidden;
}

std::vector<Vector2> Game::get_blocked() {
  std::vector<Vector2> blocked;
  for (auto &snake : m_snake_vec) {
    for (auto &i : snake.get_body()) {
      blocked.push_back(i);
    }
  }
  for (auto &wall : m_wall_vec) {
    for (auto &tile : wall.get_tiles()) {
      blocked.push_back(tile);
    }
  }
  return blocked;
}

typedef enum {
  STATE_MYHEAD = -127,
  STATE_EMPTY = 0,
  STATE_FOOD_BASELINE = 64,
  STATE_BLOCKED = 127
} Cell_state_t;

#define GET_CELL(v, x, y) (v[y * game_globals.cell_count + x])

Game_grid_linear_t Game::get_game_state() {

  std::vector<float> tmp;
  tmp.resize(game_globals.cell_count * game_globals.cell_count, STATE_EMPTY);

  std::vector<Vector2> blocked = get_blocked();
  for (auto &f : blocked) {
    if (f.x >= 0 && f.y >= 0 && f.x < game_globals.cell_count &&
        f.y < game_globals.cell_count) {
      GET_CELL(tmp, f.x, f.y) = STATE_BLOCKED;
    }
  }

  for (auto &f : m_food_vec) {
    GET_CELL(tmp, f.get_position().x, f.get_position().y) =
        (float)STATE_FOOD_BASELINE +
        (float)get_unique_food_id(f.get_category(), f.get_type());
  }

  GET_CELL(tmp, m_snake->get_head().x, m_snake->get_head().y) = STATE_MYHEAD;

  // for (int y = 0; y < game_globals.cell_count; y++) {
  //   for (int x = 0; x < game_globals.cell_count; x++) {
  //     if (GET_CELL(tmp,x,y) == STATE_EMPTY) {
  //       printf("□");
  //     } else if (GET_CELL(tmp,x,y) == STATE_BLOCKED) {
  //       printf("■");
  //     } else if (GET_CELL(tmp,x,y) == STATE_MYHEAD) {
  //       printf("★");
  //     } else {
  //       printf("∙");
  //     }
  //   }
  //   printf("\n");
  // }
  // printf("\n");

  return tmp;
}
Game_grid_t Game::get_grid_blocked() {
  std::vector<std::vector<int>> tmp;
  tmp.resize(game_globals.cell_count);
  for (auto &item : tmp)
    item.resize(game_globals.cell_count);
  for (int i = 0; i < game_globals.cell_count; i++) {
    for (int j = 0; j < game_globals.cell_count; j++) {
      tmp[i][j] = 0;
    }
  }
  std::vector<Vector2> blocked = get_blocked();
  for (auto &f : blocked) {
    if (f.x >= 0 && f.y >= 0 && f.x < game_globals.cell_count &&
        f.y < game_globals.cell_count)
      tmp[f.y][f.x] = 1;
  }
  // for (int j = 0; j < game_globals.cell_count; j++) {
  //   for (int i = 0; i < game_globals.cell_count; i++) {
  //     printf("%s", tmp[j][i]?"■":"□");
  //   }
  //   printf("\n");
  // }
  return tmp;
}

void Game::check_food_collision() {
  int collisions = 0;
  Food_cat_t collision_cat;
  std::vector<Food>::iterator collision_it;
  int collision_type;

  for (auto &snake : m_snake_vec) {
    collisions = 0;
    for (auto it = m_food_vec.begin(); it != m_food_vec.end(); it++) {
      if (Vector2Equals(snake.get_head(), it->get_position())) {
        // Mark for deletion
        collisions++;
        collision_it = it;
        collision_cat = it->get_category();
        collision_type = it->get_type();
      }
    }

    if (collisions) {

      snake.highlight_head();

      // Delete and update food
      m_food_vec.erase(collision_it);
      m_food_vec.push_back(Food(get_forbidden()));

      // Handle multiplier
      // std::cout << get_unique_food_id(collision_cat, collision_type) << " vs
      // "
      //           << m_last_food_type << std::endl;

      m_bonus_multiplier = m_next_bonus_multiplier;

      // Update streak (A surprise doesn't alter an existing streak)
      if (snake.get_controller() == PLAYER ||
          snake.get_controller() == AI_TRAIN) {
        if (collision_cat != FOOD_SURPRISE) {
          if (get_unique_food_id(collision_cat, collision_type) ==
              m_last_food_type) {
            m_streak_len++;
          } else {
            m_streak_len = 1;
          }
          m_streak_multiplier = std::max(1, m_streak_len);
          m_next_bonus_multiplier = 1;
          m_last_food_type = get_unique_food_id(collision_cat, collision_type);
        }
      }

      // Actual actions
      m_total_multiplier = m_streak_multiplier * m_bonus_multiplier;

      int prev_score = m_score;
      if (collision_cat == FOOD_REGULAR) {
        snake.mark_to_add_segment();
        if (snake.get_controller() == PLAYER ||
            snake.get_controller() == AI_TRAIN) {
          PlaySound(m_food_sound);
          m_score += (m_total_multiplier * game_globals.score_regular);
        }
      } else if (collision_cat == FOOD_BONUS) {
        snake.mark_to_remove_segments(game_globals.length_bonus);
        if (snake.get_controller() == PLAYER ||
            snake.get_controller() == AI_TRAIN) {
          PlaySound(m_hourglass_sound);
          m_score += (m_total_multiplier * game_globals.score_bonus);
        }
      } else if (collision_cat == FOOD_SURPRISE) {
        if (snake.get_controller() == PLAYER ||
            snake.get_controller() == AI_TRAIN) {
          int val = GetRandomValue(0, 100);
          if (val <= game_globals.prob_surprise_bonus) {
            PlaySound(m_bonus10x_sound);
            m_next_bonus_multiplier = 10;
          } else {
            PlaySound(m_wall_sound);
            int snake_len = snake.get_body().size();
            int max_wall_len = 3;
            if (snake_len <= 10) {
              max_wall_len = (int)(game_globals.cell_count * 0.75);
            } else if (snake_len <= 20) {
              max_wall_len = (int)(game_globals.cell_count * 0.50);
            } else if (snake_len <= 30) {
              max_wall_len = (int)(game_globals.cell_count * 0.375);
            } else if (snake_len <= 40) {
              max_wall_len = (int)(game_globals.cell_count * 0.25);
            } else {
              max_wall_len = (int)(game_globals.cell_count * 0.125);
            }
            m_wall_vec.push_back(Wall(12, get_forbidden(), snake.get_head()));
          }
        }
      }
      if (m_score >= 100 && prev_score < 100)
        spawn_in_seconds(1);
      if (m_score >= 200 && prev_score < 200)
        spawn_in_seconds(1);
      if (m_score >= 300 && prev_score < 300)
        spawn_in_seconds(1);
    }
  }
}

void Game::check_bonus_timeout() {
  int expirations = 0;
  std::vector<std::vector<Food>::iterator> expiration_it;
  for (auto it = m_food_vec.begin(); it != m_food_vec.end(); it++) {
    if (it->get_category() != FOOD_BONUS)
      continue;
    if (GetTime() >= it->get_expiration()) {
      // Mark for deletion
      expirations++;
      expiration_it.push_back(it);
    }
  }
  if (expirations) {
    for (auto &it : expiration_it) {
      m_food_vec.erase(it);
      m_food_vec.push_back(Food(get_forbidden()));
    }
  }
}

void Game::check_game_over() {

  int ii = 0;
  std::vector<int> despawn_snakes;
  for (auto &snake : m_snake_vec) {

    bool collision_with_edge = false;
    bool collision_with_tail = false;
    bool collision_with_wall = false;
    bool collision_with_enemy = false;

    if (snake.get_head().x >= game_globals.cell_count ||
        snake.get_head().x < 0 ||
        snake.get_head().y >= game_globals.cell_count ||
        snake.get_head().y < 0) {
      collision_with_edge = true;
      std::cout << "Collision of snake " << ii << " with edge at "
                << snake.get_head().x << "," << snake.get_head().y << std::endl;
    }
    std::deque<Vector2> body = snake.get_body();
    for (int i = 1; i < body.size(); i++) {
      if (Vector2Equals(snake.get_head(), body[i])) {
        collision_with_tail = true;
        std::cout << "Collision of snake " << ii << " with tail at "
                  << body[i].x << "," << body[i].y << std::endl;
      }
    }
    for (auto &wall : m_wall_vec) {
      for (auto &tile : wall.get_tiles()) {
        if (Vector2Equals(snake.get_head(), tile)) {
          collision_with_wall = true;
          std::cout << "Collision of snake " << ii << " with wall at "
                    << snake.get_head().x << "," << snake.get_head().y
                    << std::endl;
        }
      }
    }

    // Collision with other snake
    for (int jj = 0; jj < m_snake_vec.size(); jj++) {
      if (jj != ii) {
        for (auto &tile : m_snake_vec[jj].get_body()) {
          if (Vector2Equals(snake.get_head(), tile)) {
            collision_with_enemy = true;
            std::cout << "Collision of snake " << ii << " with snake " << jj
                      << " at " << snake.get_head().x << ","
                      << snake.get_head().y << std::endl;
          }
        }
      }
    }

    if (collision_with_edge || collision_with_tail || collision_with_wall ||
        collision_with_enemy) {
      if (snake.get_controller() == PLAYER ||
          snake.get_controller() == AI_TRAIN) {
        PlaySound(m_gameover_sound);
        game_over();
      } else {
        despawn_snakes.push_back(ii);
      }
    }

    // std::cout << "Snake " << ii << " |  " << collision_with_edge << " "
    //           << collision_with_tail << " " << collision_with_wall << " "
    //           << collision_with_enemy << std::endl;
    ii++;
  }

  // Despawn dead snakes and spawn new ones
  for (int kk = despawn_snakes.size() - 1; kk >= 0; kk--) {
    m_snake_vec.erase(m_snake_vec.begin() + despawn_snakes[kk]);
    spawn_in_seconds(1);
  }
  despawn_snakes.clear();
}

void Game::spawn_in_seconds(float timeout) {
  m_spawn_queue.push_back(GetTime() + timeout);
}

void Game::check_spawn() {
  float curr = GetTime();
  for (auto it = m_spawn_queue.begin(); it != m_spawn_queue.end();) {
    if (curr > (*it)) {
      m_snake_vec.push_back(spawn_snake());
      it = m_spawn_queue.erase(it);
    } else
      ++it;
  }
}

Snake Game::spawn_snake() {
  int rnd_mode =
      GetRandomValue(1, NUM_CONTROLLERS - 2); // excluding the "AI_TRAIN" mode
  Snake_ctrl_t ctrl = (Snake_ctrl_t)rnd_mode;
  // ctrl = AI_EAT_CLOSEST;
  Game_grid_t grid = get_grid_blocked();

  int best_x = 0;
  int best_cnt = 0;
  int start_search = GetRandomValue(0, game_globals.cell_count - 1);
  int x;
  for (int xt = 0; xt < game_globals.cell_count; xt++) {
    x = (start_search + xt) % game_globals.cell_count;
    int cnt = 0;
    // Count how many cells are free starting from the first row
    for (int y = 0; y < game_globals.cell_count; y++) {
      if (!grid[y][x])
        cnt++;
      if (grid[y][x])
        break;
    }
    if (cnt > best_cnt) {
      best_cnt = cnt;
      best_x = x;
    }
  }
  std::cout << "Spawning snake at x = " << best_x << " (" << best_cnt
            << " free cells)" << std::endl;

  std::deque<Vector2> body;
  body.push_back(Vector2{(float)best_x, 0});
  body.push_back(Vector2{(float)best_x, -1});
  body.push_back(Vector2{(float)best_x, -2});
  return Snake(ctrl, body);
}

void Game::game_over() {

  get_grid_blocked();
  m_running = false;
  m_paused = false;
  m_first_game = false;
  m_food_vec.clear();
  m_last_score = m_score;
  m_score = 0;
  m_streak_len = 0;
  m_streak_multiplier = 1;
  m_next_bonus_multiplier = 1;
  m_bonus_multiplier = 1;
  m_total_multiplier = 1;
  m_last_food_type = -1;
  for (int i = 0; i < NUM_FOOD_INSTANCES; i++) {
    m_food_vec.push_back(Food(get_forbidden()));
  }
  m_snake_vec.clear();
#ifndef TRAINING
  m_snake_vec.push_back(Snake(PLAYER, game_globals.initial_player_pos));
#else
  m_snake_vec.push_back(Snake(AI_TRAIN, game_globals.initial_player_pos));
#endif

  m_snake_vec.push_back(spawn_snake());
  m_snake = &(m_snake_vec[0]);
  for (auto &snake : m_snake_vec)
    snake.reset();
  m_wall_vec.clear();
}

void Game::update() {
  if (m_running) {
    for (auto &snake : m_snake_vec) {
      Game_grid_t grid_blocked = get_grid_blocked();
      Game_grid_linear_t grid_full = get_game_state();
      snake.update(grid_blocked, m_food_vec, m_snake->get_body());
    }
    check_spawn();
    check_food_collision();
    check_bonus_timeout();
    check_game_over();
  }
}

void Game::write_text() {
  if (m_running) {
    DrawText(TextFormat("Score %4i", m_score), game_globals.offset - 5,
             game_globals.offset +
                 game_globals.cell_count * game_globals.cell_size + 10,
             40, game_globals.dark_green);
    DrawText(TextFormat("Length %4i", m_snake->get_body().size()),
             game_globals.offset + 520,
             game_globals.offset +
                 game_globals.cell_count * game_globals.cell_size + 10,
             40, game_globals.dark_green);
    if (m_next_bonus_multiplier > 1) {
      DrawText(TextFormat("%ix", m_next_bonus_multiplier),
               game_globals.offset - 5, 20, 40, game_globals.dark_green);
    }
    if (m_streak_len > 1)
      DrawText(TextFormat("Streak %2i", m_streak_len),
               game_globals.offset + 550, 20, 40, game_globals.dark_green);
  }

  if (!m_running && !m_paused && !m_first_game) {
    DrawRectangle(game_globals.offset, game_globals.offset,
                  game_globals.cell_count * game_globals.cell_size,
                  game_globals.cell_count * game_globals.cell_size,
                  game_globals.green);
    DrawText("GAME OVER", game_globals.offset + 120, game_globals.offset + 300,
             80, game_globals.dark_green);
    DrawText(TextFormat("Score %4i", m_last_score), game_globals.offset + 240,
             game_globals.offset + 400, 50, game_globals.dark_green);
    DrawText("Press ENTER to restart", game_globals.offset + 150,
             game_globals.offset + 500, 35, game_globals.dark_green);
  }
  if (!m_running && !m_paused && m_first_game) {
    DrawText("Press ENTER to start", game_globals.offset + 180,
             game_globals.offset + 500, 35, game_globals.dark_green);
  }
  if (!m_running && m_paused) {
    DrawText("GAME PAUSED", game_globals.offset + 250,
             game_globals.offset + 500, 35, game_globals.dark_green);
  }
}

int main() {

  // Tell the window to use vsync and work on high DPI displays
  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

  std::cout << "Starting the game..." << std::endl;
  InitWindow(game_globals.cell_size * game_globals.cell_count +
                 2 * game_globals.offset,
             game_globals.cell_size * game_globals.cell_count +
                 2 * game_globals.offset,
             "Cool Snake");
  SetTargetFPS(60);

  load_textures();

  Game game = Game();
  while (!WindowShouldClose()) {

    BeginDrawing();

    if (IsKeyPressed(KEY_UP) && game.m_snake->get_controller() == PLAYER) {
      game.m_snake->try_move_up();
    }
    if (IsKeyPressed(KEY_DOWN) && game.m_snake->get_controller() == PLAYER) {
      game.m_snake->try_move_down();
    }
    if (IsKeyPressed(KEY_LEFT) && game.m_snake->get_controller() == PLAYER) {
      game.m_snake->try_move_left();
    }
    if (IsKeyPressed(KEY_RIGHT) && game.m_snake->get_controller() == PLAYER) {
      game.m_snake->try_move_right();
    }
    if (IsKeyPressed(KEY_ENTER)) {
      game.m_running = true;
    }
    if (IsKeyPressed(KEY_SPACE)) {
      game.m_running = !game.m_running;
      game.m_paused = !game.m_paused;
    }

    if (event_triggered(0.1)) {
      game.update();
      // game.get_game_state();
    }

    ClearBackground(game_globals.green);

    Rectangle rect = {
        (float)(game_globals.offset - 5), (float)(game_globals.offset - 5),
        (float)(game_globals.cell_count * game_globals.cell_size + 10),
        (float)(game_globals.cell_count * game_globals.cell_size + 10)};
    DrawRectangleLinesEx(rect, 5, game_globals.dark_green);

    game.draw();
    game.write_text();

    EndDrawing();
  }

  unload_textures();
  CloseWindow();
  return 0;
}
