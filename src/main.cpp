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

void load_textures() {
  for (int i = 0; i < NUM_FOOD_TYPES; i++) {
    Image image = LoadImage(
        (game_globals.img_path + "food_" + std::to_string(i) + ".png").c_str());
    Texture txt = LoadTextureFromImage(image);
    food_texture.push_back(txt);
    UnloadImage(image);
  }
}

void unload_textures() {
  for (auto &i : food_texture)
    UnloadTexture(i);
  food_texture.clear();
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

void Game::check_food_collision() {
  int collisions = 0;
  std::vector<Food>::iterator collision_it;
  for (auto it = m_food_vec.begin(); it != m_food_vec.end(); it++) {
    if (Vector2Equals(m_snake.get_head(), it->get_position())) {
      // Mark for deletion
      collisions++;
      collision_it = it;
    }
  }
  // Delete
  if (collisions) {
    m_food_vec.erase(collision_it);
    std::deque<Vector2> all_forbidden;
    all_forbidden = m_snake.get_body();
    for (auto &i : m_food_vec)
      all_forbidden.push_back(i.get_position());
    m_food_vec.push_back(Food(all_forbidden));
    m_snake.mark_to_add_segment();
    m_score++;
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
  for (int i = 0; i < NUM_FOOD_INSTANCES; i++) {
    std::deque<Vector2> all_forbidden;
    all_forbidden = m_snake.get_body();
    for (auto &j : m_food_vec)
      all_forbidden.push_back(j.get_position());
    m_food_vec.push_back(Food(all_forbidden));
  }
}

void Game::update() {
  if (m_running) {
    m_snake.update();
    check_food_collision();
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
      game.m_running = true;
    }
    if (IsKeyPressed(KEY_DOWN)) {
      game.m_snake.try_move_down();
      game.m_running = true;
    }
    if (IsKeyPressed(KEY_LEFT)) {
      game.m_snake.try_move_left();
      game.m_running = true;
    }
    if (IsKeyPressed(KEY_RIGHT)) {
      game.m_snake.try_move_right();
      game.m_running = true;
    }

    if (event_triggered(0.15))
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
    DrawText(TextFormat("Length %4i", game.m_snake.get_body().size()), game_globals.offset + 520,
             game_globals.offset +
                 game_globals.cell_count * game_globals.cell_size + 10,
             40, game_globals.dark_green);              
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
    }

    

    EndDrawing();
  }

  unload_textures();
  CloseWindow();
  return 0;
}
