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
  FOOD_BONUS = 1,
  NUM_FOOD_CATEGORIES
} Food_cat_t;

class Food {
public:
  Food(std::vector<Vector2> forbidden);
  inline Vector2 get_position() { return m_position; }
  inline Food_cat_t get_category() { return m_category; }
  inline int get_type() { return m_food_type; }
  inline double get_expiration() { return m_expiration_time; };
  void draw();

private:
  Food_cat_t m_category = FOOD_REGULAR;
  int m_food_type = 0;
  Texture m_texture;
  Vector2 m_position = {0, 0};
  double m_creation_time;
  double m_expiration_time;
};

#endif