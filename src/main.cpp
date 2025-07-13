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

void Game::draw() {
  m_snake.draw();
  for (auto &food : m_food_vec)
    food.draw();
}

std::vector<Vector2> Game::get_forbidden() {
  std::vector<Vector2> forbidden;
  for (auto &i : m_snake.get_body()) {
    forbidden.push_back(i);
  }
  for (auto &i : m_food_vec) {
    forbidden.push_back(i.get_position());
  }
  return forbidden;
}

void Game::check_food_collision() {
  int collisions = 0;
  Food_cat_t collision_cat;
  std::vector<Food>::iterator collision_it;
  int collision_type;
  for (auto it = m_food_vec.begin(); it != m_food_vec.end(); it++) {
    if (Vector2Equals(m_snake.get_head(), it->get_position())) {
      // Mark for deletion
      collisions++;
      collision_it = it;
      collision_cat = it->get_category();
      collision_type = it->get_type();
    }
  }

  if (collisions) {

    // Delete and update food
    m_food_vec.erase(collision_it);
    m_food_vec.push_back(Food(get_forbidden()));

    // Handle multiplier
    // std::cout << (100 * (collision_cat == FOOD_BONUS) + collision_type)
    //           << " vs " << m_last_food_type << std::endl;

    if ((100 * (collision_cat == FOOD_BONUS) + collision_type) ==
        m_last_food_type) {
      m_streak_len++;
      m_multiplier = std::max(1, m_streak_len);
    } else {
      m_streak_len = 1;
      m_multiplier = 1;
    }
    m_last_food_type = 100 * (collision_cat == FOOD_BONUS) + collision_type;

    // Actual actions
    if (collision_cat == FOOD_REGULAR) {
      m_snake.mark_to_add_segment();
      m_score += (m_multiplier * game_globals.score_regular);
    } else if (collision_cat == FOOD_BONUS) {
      m_snake.mark_to_remove_segments(game_globals.length_bonus);
      m_score += (m_multiplier * game_globals.score_bonus);
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

  bool collision_with_edge = false;
  bool collision_with_tail = false;
  if (m_snake.get_head().x >= game_globals.cell_count ||
      m_snake.get_head().x < 0 ||
      m_snake.get_head().y >= game_globals.cell_count ||
      m_snake.get_head().y < 0) {
    collision_with_edge = true;
  }
  std::deque<Vector2> body = m_snake.get_body();
  for (int i = 1; i < body.size(); i++) {
    if (Vector2Equals(m_snake.get_head(), body[i])) {
      collision_with_tail = true;
    }
  }

  if (collision_with_edge || collision_with_tail) {
    game_over();
  }
}

void Game::game_over() {
  m_running = false;
  m_first_game = false;
  m_snake.reset();
  m_food_vec.clear();
  m_last_score = m_score;
  m_score = 0;
  m_streak_len = 1;
  m_multiplier = 1;
  m_last_food_type = -1;
  for (int i = 0; i < NUM_FOOD_INSTANCES; i++) {
    m_food_vec.push_back(Food(get_forbidden()));
  }
}

void Game::update() {
  if (m_running) {
    m_snake.update();
    check_food_collision();
    check_bonus_timeout();
    check_game_over();
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

    if (IsKeyPressed(KEY_UP)) {
      game.m_snake.try_move_up();
    }
    if (IsKeyPressed(KEY_DOWN)) {
      game.m_snake.try_move_down();
    }
    if (IsKeyPressed(KEY_LEFT)) {
      game.m_snake.try_move_left();
    }
    if (IsKeyPressed(KEY_RIGHT)) {
      game.m_snake.try_move_right();
    }
    if (IsKeyPressed(KEY_ENTER)) {
      game.m_running = true;
    }

    if (event_triggered(0.1))
      game.update();

    ClearBackground(game_globals.green);

    Rectangle rect = {
        (float)(game_globals.offset - 5), (float)(game_globals.offset - 5),
        (float)(game_globals.cell_count * game_globals.cell_size + 10),
        (float)(game_globals.cell_count * game_globals.cell_size + 10)};
    DrawRectangleLinesEx(rect, 5, game_globals.dark_green);

    game.draw();

    if (game.m_running) {
      DrawText(TextFormat("Score %4i", game.m_score), game_globals.offset - 5,
               game_globals.offset +
                   game_globals.cell_count * game_globals.cell_size + 10,
               40, game_globals.dark_green);
      DrawText(TextFormat("Length %4i", game.m_snake.get_body().size()),
               game_globals.offset + 520,
               game_globals.offset +
                   game_globals.cell_count * game_globals.cell_size + 10,
               40, game_globals.dark_green);
      if (game.m_multiplier > 1) {
        DrawText(TextFormat("%ix", game.m_multiplier), game_globals.offset - 5,
                 20, 40, game_globals.dark_green);
      }
    }

    if (!game.m_running && !game.m_first_game) {
      DrawRectangle(game_globals.offset, game_globals.offset,
                    game_globals.cell_count * game_globals.cell_size,
                    game_globals.cell_count * game_globals.cell_size,
                    game_globals.green);
      DrawText("GAME OVER", game_globals.offset + 120,
               game_globals.offset + 300, 80, game_globals.dark_green);
      DrawText(TextFormat("Score %4i", game.m_last_score),
               game_globals.offset + 240, game_globals.offset + 400, 50,
               game_globals.dark_green);
      DrawText("Press ENTER to restart", game_globals.offset + 150,
               game_globals.offset + 500, 35, game_globals.dark_green);
    }
    if (!game.m_running && game.m_first_game) {
      DrawText("Press ENTER to start", game_globals.offset + 180,
               game_globals.offset + 500, 35, game_globals.dark_green);
    }

    EndDrawing();
  }

  unload_textures();
  CloseWindow();
  return 0;
}
