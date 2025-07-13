#include "food.h"

#include <iostream>

extern game_globals_struct game_globals;
extern std::vector<Texture> food_texture;
extern Texture hourglass_texture;

bool vector2_in_vector(Vector2 element, std::vector<Vector2> vec) {
  for (auto &i : vec)
    if (Vector2Equals(element, i))
      return true;

  return false;
}

Food::Food(std::vector<Vector2> forbidden) {

  int decision_var = GetRandomValue(0, 100);
  if (decision_var <= game_globals.prob_bonus) {
    m_category = FOOD_BONUS;
  } else {
    m_category = FOOD_REGULAR;
  }

  do {
    m_position.x = GetRandomValue(0, game_globals.cell_count - 1);
    m_position.y = GetRandomValue(0, game_globals.cell_count - 1);
  } while (
      vector2_in_vector(m_position, forbidden)); // check for forbidden positions

  if (m_category == FOOD_REGULAR) {
    m_food_type = GetRandomValue(0, NUM_FOOD_TYPES - 1);
    m_texture = food_texture[m_food_type];
  } else if (m_category == FOOD_BONUS) {
    m_texture = hourglass_texture;
  } else { /* TODO: additional bonus/malus */
  }

  m_creation_time = GetTime();
  if (m_category == FOOD_BONUS) {
    m_expiration_time = m_creation_time + game_globals.timeout_bonus;
  }
}

void Food::draw() {
  double alpha = 255.0;
  if (m_category == FOOD_BONUS) {
    alpha =
        (get_expiration() - GetTime()) / game_globals.timeout_bonus * 192 + 64;
  }
  DrawTexture(m_texture,
              m_position.x * game_globals.cell_size + game_globals.offset,
              m_position.y * game_globals.cell_size + game_globals.offset,
              {255, 255, 255, (unsigned char) alpha});
}
