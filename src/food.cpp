#include "food.h"

#include <iostream>

extern game_globals_struct game_globals;
extern std::vector<Texture> food_texture;

bool vector2_in_deque(Vector2 element, std::deque<Vector2> deque) {
  for (auto &i : deque)
    if (Vector2Equals(element, i))
      return true;

  return false;
}

Food::Food(std::deque<Vector2> forbidden) {

  do {
    m_position.x = GetRandomValue(0, game_globals.cell_count - 1);
    m_position.y = GetRandomValue(0, game_globals.cell_count - 1);
  } while (
      vector2_in_deque(m_position, forbidden)); // check for forbidden positions

  if (m_category == FOOD_REGULAR) {
    int food_type = GetRandomValue(0, NUM_FOOD_TYPES - 1);
    m_texture = food_texture[food_type];
  } else { /* TODO: additional bonus/malus */
  }
}

void Food::draw() {
  DrawTexture(
      m_texture, m_position.x * game_globals.cell_size + game_globals.offset,
      m_position.y * game_globals.cell_size + game_globals.offset, WHITE);
}
