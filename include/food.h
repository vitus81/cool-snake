#ifndef __FOOD_H__
#define __FOOD_H__

#include <deque>
#include <vector>

#include "constants.h"
#include "raylib.h"
#include "raymath.h"

#define NUM_FOOD_TYPES (4)

typedef enum {
  FOOD_REGULAR = 0,
  FOOD_BONUS,
  FOOD_MALUS,
  FOOD_SURPRISE
} Food_cat_t;

class Food {
public:
  Food(std::deque<Vector2> forbidden);
  inline Vector2 get_position() { return m_position; }
  void draw();

private:
  Food_cat_t m_category = FOOD_REGULAR;
  Texture m_texture;
  Vector2 m_position = {0, 0};
};

#endif