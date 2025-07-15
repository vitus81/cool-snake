#ifndef __WALL_H__
#define __WALL_H__

#include "constants.h"
#include "raylib.h"
#include "raymath.h"

#include <vector>

typedef enum { EAST_WEST, NORTH_SOUTH, UNDEFINED } Wall_direction_t;

class Wall {
public:  
  // Create fixed wall from start tile, num of tiles and direction
  Wall(Vector2 start_tile, int num_tiles, Wall_direction_t direction);
  // Create random wall with constraints
  Wall(int max_len, std::vector<Vector2> forbidden, Vector2 snake_head);

  inline std::vector<Vector2> get_tiles() { return m_tiles; };
  void draw();

private:
  std::vector<Vector2> m_tiles;
  int m_direction = UNDEFINED;
};

#endif