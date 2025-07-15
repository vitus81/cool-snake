#include "raylib.h"

#include "wall.h"

#include <iostream>

extern game_globals_struct game_globals;

bool is_snake_distance_ok(std::vector<Vector2> tiles, Vector2 snake_head) {
  bool is_ok = true;
  float threshold = 100.0f;
  for (auto &i : tiles) {
    float dist;
    dist = powf32(i.x - snake_head.x, 2) + powf32(i.y - snake_head.y, 2);
    if (dist < threshold) {
      is_ok = false;
      break;
    }
  }
  return is_ok;
}

int vector2_intersect_size(std::vector<Vector2> &vec1,
                           std::vector<Vector2> &vec2) {
  int int_size = 0;
  for (auto &v1 : vec1)
    for (auto &v2 : vec2)
      int_size += Vector2Equals(v1, v2);

  return int_size;
}

std::vector<Vector2> get_wall_tiles(Vector2 start_tile, int num_tiles,
                                    Wall_direction_t direction) {
  std::vector<Vector2> tiles;
  if (num_tiles > 1) {
    tiles.push_back(start_tile);
    for (int i = 1; i < num_tiles; i++) {
      Vector2 next_tile;
      if (direction == NORTH_SOUTH) {
        next_tile.x = start_tile.x;
        next_tile.y = start_tile.y + i;
      }
      if (direction == EAST_WEST) {
        next_tile.x = start_tile.x + i;
        next_tile.y = start_tile.y;
      }
      if (next_tile.x < game_globals.cell_count &&
          next_tile.y < game_globals.cell_count) {
        tiles.push_back(next_tile);
      }
    }
  }
  return tiles;
}

// Fixed wall
Wall::Wall(Vector2 start_tile, int num_tiles, Wall_direction_t direction) {
  m_tiles = get_wall_tiles(start_tile, num_tiles, direction);
  m_direction = direction;
}

// Random wall
Wall::Wall(int max_len, std::vector<Vector2> forbidden, Vector2 snake_head) {

  bool valid = false;
  const int max_trials = 100;
  int trials = 0;
  int len = 0;

  int wall_exclude = 3;

  bool is_collision_ok = false;
  bool is_distance_ok = false;

  std::vector<Vector2> tiles;
  Wall_direction_t direction = UNDEFINED;

  while (!valid && trials < max_trials) {
    len = GetRandomValue(2, max_len);
    direction = static_cast<Wall_direction_t>(GetRandomValue(0, 1));
    Vector2 start_tile;
    start_tile.x =
        GetRandomValue(wall_exclude, game_globals.cell_count - wall_exclude);
    start_tile.y =
        GetRandomValue(wall_exclude, game_globals.cell_count - wall_exclude);
    tiles = get_wall_tiles(start_tile, len, direction);

    is_distance_ok = is_snake_distance_ok(tiles, snake_head);
    is_collision_ok = (vector2_intersect_size(tiles, forbidden) == 0);
    valid = (is_collision_ok && is_distance_ok);
    trials++;
  }

  if (tiles.size() > 0) {
    m_tiles = tiles;
    m_direction = direction;
  } else {
    std::cout << "Impossible to find a valid wall after " << trials << "trials"
              << std::endl;
  }
}

void Wall::draw() {

  //   for (auto &i : m_tiles) {
  //     DrawRectangle(i.x * game_globals.cell_size + game_globals.offset,
  //                   i.y * game_globals.cell_size + game_globals.offset,
  //                   game_globals.cell_size, game_globals.cell_size, YELLOW);
  //   }

  Vector2 start, end;
  start.x = m_tiles.begin()->x * game_globals.cell_size + game_globals.offset;
  start.y = m_tiles.begin()->y * game_globals.cell_size + game_globals.offset;
  end.x = (m_tiles.end() - 1)->x * game_globals.cell_size + game_globals.offset;
  end.y = (m_tiles.end() - 1)->y * game_globals.cell_size + game_globals.offset;

  if (m_direction == NORTH_SOUTH) {
    end.y += game_globals.cell_size;
    start.x += game_globals.cell_size / 2;
    end.x += game_globals.cell_size / 2;
  }
  if (m_direction == EAST_WEST) {
    end.x += game_globals.cell_size;
    start.y += game_globals.cell_size / 2;
    end.y += game_globals.cell_size / 2;
  }

  DrawLineEx(start, end, 5, game_globals.dark_green);
}